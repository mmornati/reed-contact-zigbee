# 3. Firmware: build and flash

Sketch: [`firmware/reed_contact/reed_contact.ino`](../firmware/reed_contact/reed_contact.ino)

## What it does

- Starts as a **Zigbee router**, so it stays always on and relays traffic for other devices.
- Switches the radio to the **external U.FL antenna** (GPIO3 = LOW, GPIO14 = HIGH).
- Exposes one **IAS Zone contact** endpoint (manufacturer `DIY`, model `C6ReedContact`).
- Reads the reed on **D2 / GPIO2**: `LOW` = fully closed, `HIGH` = not closed. Debounced for 200 ms.
- Sends the state on every change, and again **every 5 minutes** as a heartbeat.
- Handles IAS Zone enrollment. It remembers the enrollment across reboots and retries every minute if the coordinator didn't answer.
- **LED:** slow blink = looking for a network, off = joined. While you hold BOOT it blinks fast.
- **Factory reset:** hold **BOOT** for 3 s. The board leaves the network, forgets the enrollment and reboots ready to pair.

## Option A: PlatformIO (recommended, native on Apple Silicon)

[`firmware/platformio.ini`](../firmware/platformio.ini) holds all the build settings (board, Zigbee router mode, partition table), so there's nothing to pick from menus. It uses the community [pioarduino](https://github.com/pioarduino/platform-espressif32) platform, which ships the arduino-esp32 3.x core. The official PlatformIO platform stops at core 2.x, which has no Zigbee.

1. Install PlatformIO Core. It needs Python 3.10–3.14; Homebrew's `platformio` runs on a newer Python that pioarduino rejects, so use uv:

   ```bash
   uv tool install --python 3.13 platformio
   ```

   Or install the *PlatformIO IDE* extension in VS Code, which brings its own Python.
2. Build (the first run downloads the ESP32 platform and toolchain, a few hundred MB):

   ```bash
   cd firmware
   pio run
   ```

3. Connect the XIAO over USB-C (with **no** 12 V on the buck). For the very first flash, erase it, then upload:

   ```bash
   pio run -t erase
   pio run -t upload
   ```

   If the port isn't found: hold **BOOT**, tap **RESET**, release **BOOT**, then upload again.
4. Watch the logs:

   ```bash
   pio device monitor
   ```

## Option B: Arduino IDE

> On an Apple Silicon Mac, the Arduino IDE needs Rosetta: its ctags preprocessing tool is Intel-only and fails with *"bad CPU type in executable"*. Install it with `softwareupdate --install-rosetta --agree-to-license`, or use PlatformIO above.

1. Install **Arduino IDE 2.x**.
2. *File → Preferences → Additional boards manager URLs*:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. *Boards Manager* → install **esp32 by Espressif Systems**, version **3.3.0 or newer**.
4. Open `firmware/reed_contact/reed_contact.ino`.
5. *Tools* menu:

| Setting | Value |
|---|---|
| Board | **XIAO_ESP32C6** |
| Zigbee mode | **Zigbee ZCZR (coordinator/router)** |
| Partition Scheme | **Zigbee ZCZR 4MB with spiffs** |
| Erase All Flash Before Sketch Upload | **Enabled** for the first flash, then Disabled |
| USB CDC On Boot | Enabled (so you see the logs) |

If you pick the wrong Zigbee mode, the sketch refuses to compile with a clear message.

Then flash:

1. Connect the XIAO over USB-C (with **no** 12 V on the buck).
2. Click **Upload**. If the port doesn't show up: hold **BOOT**, tap **RESET**, release **BOOT**, then upload again.
3. Open the Serial Monitor at 115200 baud.

## First boot

Whichever tool you used, the serial log should show:

```
Starting Zigbee (router)
Joining network.......
```

Now pair it in Zigbee2MQTT. See [the next guide](04-zigbee2mqtt-and-home-assistant.md).

## Settings you might change

At the top of the sketch:

| Define | Default | Meaning |
|---|---|---|
| `REED_PIN` | `2` (D2) | Reed input pin |
| `DEBOUNCE_MS` | `200` | How long the reed must stay stable |
| `HEARTBEAT_MS` | 5 min | Periodic state resend |
| `FACTORY_RESET_HOLD_MS` | `3000` | BOOT hold time for reset |

To use the onboard ceramic antenna instead, remove the `useExternalAntenna()` call.

## Updating later

Unplug the 12 V, connect USB, and upload **without erasing**: plain `pio run -t upload`, or in the Arduino IDE with *Erase All Flash = Disabled*. The board keeps its network keys and enrollment, and reconnects by itself without re-pairing.
