# 1. Hardware and wiring

> ⚠️ The YAVIS PSU is mains powered (90–260 V AC). Switch off the circuit breaker before touching its input terminals. Everything else in this project is low voltage (12 V / 5 V / 3.3 V).

## Power budget

| Load | Typical | Peak |
|---|---|---|
| XIAO ESP32-C6, Zigbee router (radio always on) | ~30 mA @ 5 V | ~120 mA @ 5 V |
| ZB-SW01 relay | ~30 mA @ 12 V | ~80 mA @ 12 V (coil on) |
| Access-control kit (keypad + 180 kg maglock) | ~500 mA @ 12 V | ~600 mA |

Total is well under 1 A. The 5 A PSU has plenty of headroom.

## Answers to the power questions

- **ZB-SW01 on 12 V directly?** Yes. It accepts 5–32 V DC on its power terminals. Check the label on your unit (some batches print "DC 7–32 V" or also accept AC) and respect its +/− marking.
- **XIAO on 12 V directly?** **No.** The XIAO's 5 V pin must stay at about 5 V, so the LM2596 buck is required. Feed the buck's 5.0 V into the XIAO **5V** pin, not 3V3. The XIAO's own regulator then makes a clean 3.3 V.
- **Which YAVIS terminals?** Access-control PSUs have a **constant** DC output (`+12V`/`DC+` and `GND`/`DC−`) and a **switched lock** output (`NO`/`COM`/`NC`, `PUSH`, delay timer). Use only the constant DC terminals. The lock output goes dead whenever the lock releases and would reboot the sensor.

### Do I need the battery?

No. The sensor runs on mains as a Zigbee router, which is the better setup: no battery to recharge, and it strengthens the mesh. A LiPo on the XIAO battery pads would only act as a backup during a power cut, but the door motor and opener also stop during a cut, and LiPo cells don't like a garage that gets cold in winter and hot in summer. Keep it for another project.

## Wiring diagram

```
          230 V AC                                   GARAGE DOOR MOTOR
             │                                      ┌────────────────────┐
     ┌───────┴────────┐                             │  push-button input │
     │ YAVIS 12V PSU  │                             │  (START/PB  COM)   │
     │                │                             └─────┬────────┬─────┘
     │  +12V   GND    │                                   │        │   ← in parallel with the
     └───┬──────┬─────┘                                   │        │     existing wall button
         │      │                                     ┌───┴────────┴───┐
         │      ├────────────────────────────────────►│ −   ZB-SW01    │
         ├──[F 1A]───────────────────────────────────►│ +   NO   COM   │
         │      │                                     └────────────────┘
         │      │       ┌──────────────────┐
         ├─────────────►│ IN+   LM2596     │
         │      └──────►│ IN−   set 5.0 V  │
         │              │ OUT+       OUT−  │
         │              └──┬──────────┬────┘
         │                 │          │
         │          ┌──────┴──────────┴───────────────┐
         │          │   5V           GND               │
         │          │                                  │
         │          │      Seeed XIAO ESP32-C6         │
         │          │                                  │
         │          │   3V3 ──[10 kΩ]──┐               │
         │          │                  │               │
         │          │   D2 (GPIO2) ────┼──[100 nF]── GND
         │          │                  │               │
         │          │   GND            │      U.FL ◄── external antenna
         │          └────┬─────────────┼───────────────┘
         │               │             │
         │               │   twisted pair to the door
         │               │             │
         │            ┌──┴─────────────┴──┐
         │            │ COM      NO       │  Reed switch (frame)
         │            └───────────────────┘
         │                  ▲ magnet (door): within ~15 mm when FULLY closed
```

### Connection table

| From | To | Wire |
|---|---|---|
| YAVIS `+12V` (constant) | Fuse 1 A → LM2596 `IN+` | red, 0.5 mm² |
| YAVIS `GND` | LM2596 `IN−` | black, 0.5 mm² |
| YAVIS `+12V` (constant) | ZB-SW01 `+` / `DC+` | red |
| YAVIS `GND` | ZB-SW01 `−` / `DC−` | black |
| LM2596 `OUT+` (**5.0 V**) | XIAO `5V` | red |
| LM2596 `OUT−` | XIAO `GND` | black |
| Reed `COM` | XIAO `GND` | cable core 1 |
| Reed `NO` | XIAO `D2` (GPIO2) | cable core 2 |
| 10 kΩ resistor | between XIAO `D2` and `3V3` | on the XIAO side |
| 100 nF capacitor | between XIAO `D2` and `GND` | on the XIAO side |
| ZB-SW01 `COM` + `NO` | motor push-button terminals | as in the motor manual |
| U.FL antenna | XIAO antenna connector | |

### Why NO and not NC?

With the reed's **NO** contact, the switch closes only when the magnet is in front of it, which is when the door is fully closed. The XIAO then sees `D2 = LOW`, meaning closed. Every failure (door ajar, cable cut, connector loose, sensor dead) gives `D2 = HIGH`, meaning **not closed**, so you get an alert instead of a false "all good".

Check this with a multimeter before mounting. These cheap reed switches don't always label the wires the same way:

1. Set the meter to continuity.
2. Find the pair that **beeps only when the magnet is close**. Those are COM + NO.
3. The pair that beeps when the magnet is **away** is COM + NC. Leave NC unconnected and insulated.

### Why the 10 kΩ + 100 nF?

The firmware already enables the ESP32's internal pull-up (about 45 kΩ). On a few metres of cable running next to a motor, that's weak and picks up noise. A 10 kΩ external pull-up plus 100 nF to ground makes the signal solid. The firmware also debounces for 200 ms.

## ZB-SW01 on the motor

- Wire the ZB-SW01 **COM / NO** to the motor's push-button input (often labelled `START`, `PP`, `P/P`, `PB`, `OPEN` + `COM`/`GND`). Put it **in parallel** with the existing wall button, so both still work.
- Use **inching (momentary) mode** if the device offers it. The HA script turns it off after 800 ms anyway, so it works either way.
- Never connect the ZB-SW01 contacts to the motor's power terminals. It is a dry contact that only simulates a button press.
