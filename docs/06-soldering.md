# 6. Soldering the XIAO

🌐 **English** · [Français](fr/06-soldering.md) · [Italiano](it/06-soldering.md)

The XIAO ESP32-C6 is small: its pads sit 2.54 mm apart and `5V`, `GND` and `3V3` are right next to each other. Most "it works, then it doesn't" problems with this sensor come from one bad joint on those pads or a wire that breaks where it leaves the solder. This guide shows a layout that avoids both.

## The short version

1. **Don't solder field wires to the XIAO.** Solder pin headers on the XIAO, plug or solder it onto a small perfboard, and bring the reed cable and the 5 V into **screw terminals** on that perfboard.
2. **Flux, a clean tip and 2–3 seconds per joint.** If the solder doesn't flow in that time, stop, add flux and try again. Don't keep heating.
3. **Measure before you power it.** Check for shorts between `5V`, `3V3` and `GND` with a multimeter every time you solder something.

## Tools and materials

| Item | Recommended | Why |
|---|---|---|
| Soldering iron | Temperature-controlled, 40–80 W, small chisel tip (1.5–2 mm) | A conical "needle" tip holds too little heat for header pins |
| Temperature | **330–350 °C** for leaded solder, **350–370 °C** for lead-free | Hotter burns the flux before the solder flows and can lift pads |
| Solder | 0.5–0.8 mm, with flux core. 63/37 leaded is the easiest to use | Thick solder makes bridges on 2.54 mm pads |
| Flux | No-clean flux pen or gel | The main fix for dull, grainy joints |
| Tip cleaner | Brass wool | A wet sponge cools the tip and shortens its life |
| Holder | Helping hands, or a breadboard to hold headers straight | Both hands free, pins at a right angle |
| Perfboard | Small piece, 2.54 mm pitch | Carries the XIAO, resistor, capacitor and terminals |
| Screw terminals | 2-pin, 5.08 mm or 3.5 mm pitch, ×2 | Reed cable and 5 V in |
| Multimeter | With continuity beeper | Every check below |
| Isopropyl alcohol + old toothbrush | 90 % or more | Cleans flux residue between pads |

## XIAO pins you use

Top view, USB-C at the top:

```
            ┌──[ USB-C ]──┐
   D0  ●    │             │    ●  5V    ← buck OUT+
   D1  ●    │   XIAO      │    ●  GND   ← buck OUT−, reed COM, 100 nF
   D2  ●    │  ESP32-C6   │    ●  3V3   ← 10 kΩ
   D3  ●    │             │    ●  D10
   D4  ●    │             │    ●  D9
   D5  ●    │             │    ●  D8
   D6  ●    │     U.FL ◯  │    ●  D7
            └─────────────┘
   D2 ← reed NO, 10 kΩ, 100 nF
```

Only four pins carry anything: `D2`, `5V`, `GND`, `3V3`. `5V`/`GND`/`3V3` are three neighbouring pads, so a bridge between them is the classic mistake. Leave the battery pads on the back alone.

## Recommended layout: XIAO on a perfboard

```
   perfboard (top view)
   ┌───────────────────────────────────────────────┐
   │   D2 ●─────┬───────────[10 kΩ]────────● 3V3   │
   │            │                                  │
   │          [100 nF]                             │
   │            │                                  │
   │  GND ●─────┴──────────────────────────● GND   │
   │                                               │
   │  [screw terminal: D2 | GND]  ← reed cable     │
   │  [screw terminal: 5V | GND]  ← buck output    │
   └───────────────────────────────────────────────┘
      XIAO plugged on headers in the middle
```

`D2` and `3V3` are on opposite sides of the XIAO, which is why the resistor goes on the perfboard and not across the board itself.

## Step by step

Unplug USB and 12 V before you start. Never solder a powered board.

### 1. Headers on the XIAO

1. Push two 7-pin male headers into a breadboard, long side down, 7 holes apart (the XIAO width). The breadboard keeps them straight.
2. Lay the XIAO on top, component side up.
3. Put flux on the pads. Solder **one corner pin** on each side, then check the board sits flat. If not, reheat that pin and press the board down.
4. Solder the rest. Touch the tip to **pad and pin together**, wait 1 second, feed solder into the joint (not onto the tip), remove the solder, then the iron. 2–3 s in total.
5. A good joint is a shiny, small cone that wraps the pin and covers the pad. A ball sitting on top, or a dull grey surface, is a cold joint: add flux and reheat.

> If you'd rather skip this step, Seeed also sells the XIAO with headers already fitted.

### 2. Parts on the perfboard

1. Solder female headers on the perfboard for the XIAO (so it stays removable for reflashing), or solder the XIAO's headers straight in.
2. Fit the **10 kΩ** between the `D2` row and the `3V3` row, and the **100 nF** between the `D2` row and a `GND` row. Bend the legs flat on the back and use them as tracks.
3. Fit the two screw terminals and join them to `D2`/`GND` and `5V`/`GND` with the component legs or short pieces of solid wire.
4. Keep `5V` and `GND` tracks at least one hole apart. Cut off leftover legs so they can't touch later.

### 3. Wires into the terminals

1. Use **stranded** wire for the reed cable and the 5 V feed. Solid core breaks after a few months of vibration.
2. Strip 6–7 mm. Either use ferrules, or twist the strands and screw them in **without** tinning them first: tinned wire in a screw terminal loosens over time.
3. If you have to solder a wire to the reed switch leads, slide **heat-shrink** on first, tin both ends, join them, then shrink the tube over the joint. Add a dab of hot glue or a cable tie so the cable pulls on the tie, not on the joint.

### 4. Clean

Scrub the joints with isopropyl alcohol and a toothbrush. Flux residue between `D2` and `GND` can leak a little current in a damp garage and make the input unstable.

## Check before powering

With the multimeter, power off, XIAO in place:

| Measure between | Expected | If not |
|---|---|---|
| `5V` and `GND` | No beep (not a short) | Look for a bridge on the right-hand pads or the terminal |
| `3V3` and `GND` | No beep | Same |
| `D2` and `3V3` | About **10 kΩ** (a little less is normal with the XIAO fitted) | Resistor joint open, or in the wrong row |
| `D2` and `GND`, magnet in front of the reed | Beep (about 0 Ω) | Reed on NC instead of NO, or a broken joint on the cable |
| `D2` and `GND`, magnet away | No beep | Reed wires on the wrong pair, or a bridge `D2`–`GND` |
| Wiggle each wire while measuring | Reading doesn't change | Cold joint or broken strand: redo it |

Then power it from USB first, check it joins and that `contact` flips, and only then move to 12 V.

## Common problems

| Symptom | Likely cause | Fix |
|---|---|---|
| Works on the bench, flickers or drops in the box | Cold joint or a wire broken at the joint | Wiggle test above; reflow with flux; move field wires to screw terminals |
| `contact` stuck on *open* | Open joint on `D2` or on the reed cable | Continuity `D2`–`GND` with the magnet in front |
| `contact` stuck on *closed* | Bridge `D2`–`GND`, or flux residue | Inspect with a magnifier, clean with alcohol |
| XIAO doesn't power up, buck gets warm | Bridge `5V`–`GND` | Remove the solder with flux and desoldering braid |
| Pad came off the XIAO | Too hot, too long, or a wire was pulled | Lower the temperature and use headers. If `D2` is gone, wire the reed to `D1` (GPIO1) and change `REED_PIN` in the sketch, or `pin` in the ESPHome YAML |
| Joint looks like a ball on top of the pad | Pad not heated, solder only melted on the tip | Heat pad and pin together, then feed solder |
| Solder won't stick, tip is black | Oxidised tip | Clean in brass wool, re-tin the tip; keep it tinned when parked |
| Zigbee range got worse | Heat or solder near the U.FL connector | Keep the iron away from the antenna corner; reseat the U.FL plug |

Still stuck? See [Troubleshooting](05-troubleshooting.md).
