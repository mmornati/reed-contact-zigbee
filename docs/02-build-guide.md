# 2. Step-by-step build

🌐 **English** · [Français](fr/02-build-guide.md) · [Italiano](it/02-build-guide.md)

Do these steps in order. You test each part on the bench before it goes into the garage.

## Step 1: Flash and pair on the bench (USB only)

1. Flash the firmware over USB-C. See [Firmware](03-firmware.md).
2. Install the Z2M converter and pair the board. See [Zigbee2MQTT and HA](04-zigbee2mqtt-and-home-assistant.md).
3. Connect the reed switch to `D2` / `GND` with short wires. Move the magnet towards and away from it and check that Z2M shows `contact: true` (magnet close) and `contact: false` (magnet away).

You now have a working sensor before soldering anything permanent.

## Step 2: Set the buck converter to 5.0 V

1. Connect the LM2596 `IN+`/`IN−` to the 12 V supply. **Nothing on the output yet.**
2. Put a multimeter on `OUT+`/`OUT−`.
3. Turn the blue trimmer (it usually needs many turns, counter-clockwise lowers the voltage) until it reads **5.0–5.1 V**.
4. Disconnect the power. Put a drop of nail polish or hot glue on the trimmer so it can't drift.

> Never connect the XIAO before you have measured the buck output. These modules often ship at 12–35 V out.

## Step 3: Prepare the enclosure

1. Plan the layout in the IP67 box: buck on one side, XIAO on the other, antenna along the lid or a side wall, away from the buck's inductor.
2. Drill holes for **2 cable glands** at the **bottom** of the box: one for 12 V in, one for the reed cable. With glands at the bottom, water can't run in.
3. Mount the boards on the perforated back plate with M2/M3 nylon standoffs, or with double-sided foam tape for the XIAO.
4. Stick the U.FL antenna inside the lid or a side wall. ABS plastic lets Zigbee through. Don't put the box **inside** the motor's metal housing, or right against a metal door frame.

## Step 4: Wire the box

Follow the [wiring diagram](01-hardware-and-wiring.md#wiring-diagram). New to soldering, or joints that don't hold? Read the [soldering guide](06-soldering.md) first: it puts the XIAO on a perfboard with screw terminals, which is much more reliable than wires soldered straight onto its pads.

1. Solder the 10 kΩ resistor between `D2` and `3V3`, and the 100 nF capacitor between `D2` and `GND`, on the XIAO pins (or on a small piece of perfboard).
2. Buck `OUT+` → XIAO `5V`, buck `OUT−` → XIAO `GND`. Solder these, don't rely on loose Dupont jumpers in a garage.
3. Fit the screw terminal or Wago for the reed cable (`D2`, `GND`).
4. Fit the screw terminal or Wago for 12 V in, with the optional 1 A fuse on the `+`.
5. Bench test: power the box from 12 V. The XIAO LED blinks slowly while it rejoins, then turns off. The reed still works in Z2M.

> Don't plug the XIAO into USB while the box is powered from 12 V. USB 5 V and the buck's 5 V meet on the same pin. Unplug the 12 V first if you need to reflash.

## Step 5: Mount the reed switch on the door

The goal is to detect **fully closed only**, so a door stopped a few centimetres early still counts as "not closed".

1. Close the door completely.
2. Choose a spot where door and frame are closest **only** at the very end of the travel:
   - **Sectional or up-and-over door:** on the side rail near the floor, or on the frame at the top edge of the door.
   - **Roller door:** the bottom slat at floor level, with the magnet on the bottom slat and the reed on the guide rail.
3. Fix the reed (with the cable) on the **fixed frame**. Fix the **magnet** on the **moving door**, aligned with it.
4. Keep the gap under about 15 mm. Test with the multimeter, or watch Z2M: open the door by 3–5 cm and the state must already switch to `contact: false`. If it doesn't, move the parts so they line up more precisely, or reduce the gap.
5. Run the cable to the box in a conduit or with clips, away from the motor power cable.

## Step 6: Wire power and the opener

1. **Switch off the breaker.**
2. Take the 12 V from the YAVIS **constant** `+12V`/`GND` terminals. Run one pair to the sensor box and one pair to the ZB-SW01 (or daisy-chain them at the box).
3. ZB-SW01 `COM`/`NO` → the motor's push-button terminals, in parallel with the wall button (skip this if already done).
4. Close all glands and the lid.
5. Switch the breaker back on.

## Step 7: Final checks

| Test | Expected |
|---|---|
| Door fully closed | HA `Garage door` = **Closed** |
| Open the door | HA = **Open** within about 1 s |
| Stop the door 5 cm before closed | HA stays **Open** |
| `cover.close_cover` from HA | Door closes, no notification |
| Block the door (or stop it) during a close from HA | Notification after 90 s: "still not fully closed" |
| Leave it open for 10 min | "left open" notification |
| Pull the 12 V for 20 min | "sensor offline" notification |
| Z2M map | The sensor shows up as a **router** |
