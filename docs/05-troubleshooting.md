# 5. Troubleshooting

| Symptom | Likely cause | Fix |
|---|---|---|
| Compile error *"Select Tools → Zigbee mode…"* | Wrong Zigbee mode | Tools → Zigbee mode → **Zigbee ZCZR**, Partition → **Zigbee ZCZR 4MB with spiffs** |
| Build fails with *ctags: bad CPU type in executable* (Apple Silicon Mac) | Arduino's bundled ctags is Intel-only | Build with PlatformIO instead (see [Firmware](03-firmware.md)), or install Rosetta: `softwareupdate --install-rosetta --agree-to-license` |
| PlatformIO: *Python version must be 3.10 … 3.14* | Homebrew's `platformio` uses a newer Python | `brew uninstall platformio`, then `uv tool install --python 3.13 platformio` |
| Compile error on `requestIASZoneEnroll` / `restoreIASZoneEnroll` | Old ESP32 core | Update **esp32 by Espressif** to 3.3.0 or newer |
| LED blinks slowly forever | Not joining | Turn on *Permit join* in Z2M; move closer to a router; hold BOOT 3 s to factory reset; reflash with *Erase All Flash = Enabled* |
| Z2M shows *Unsupported* / *DIY C6ReedContact not supported* | Converter not loaded | File must be in `external_converters/` with a `.mjs` extension; restart Z2M; check the log for errors |
| Paired, but `contact` never changes | IAS enrollment didn't complete | Serial log shows "Enrollment timed out": in Z2M, *Reconfigure* the device, or remove it, hold BOOT 3 s and pair again |
| State inverted (closed shows open) | Reed wired on **NC** | Move the wire to **NO** (see the [multimeter check](01-hardware-and-wiring.md#why-no-and-not-nc)) |
| State flickers when the motor runs | Noise on a long reed cable | Add the 10 kΩ pull-up + 100 nF; use twisted or shielded cable; route it away from the motor's mains cable; raise `DEBOUNCE_MS` |
| Shows *closed* while the door is a few cm open | Magnet gap too forgiving | Move the reed/magnet so they only line up at the last centimetre, or reduce the gap |
| Sensor drops off when the keypad opens the door | Powered from the YAVIS **lock** output | Move to the **constant** `+12V`/`GND` terminals |
| XIAO resets or gets hot | Buck output wrong | Measure: it must be 5.0–5.1 V; re-seal the trimmer |
| Poor LQI | Antenna | Check the U.FL connector is seated; keep the antenna away from the buck and metal; check that `useExternalAntenna()` runs |
| Cover always *unavailable* in HA | Entity IDs differ | Fix the IDs in `garage_door.yaml`; Z2M friendly names should be `garage_door_sensor` and `garage_opener` |

## Useful serial log lines

```
Starting Zigbee (router)
Joining network.....
Requesting IAS Zone enrollment     (first pairing)
Restoring IAS Zone enrollment      (after a reboot)
Enrolled
Contact CLOSED / Contact OPEN
```
