# 3. Firmware: build and flash

🌐 **English** · [Français](fr/03-firmware.md) · [Italiano](it/03-firmware.md)

Sketch: [`firmware/reed_contact/reed_contact.ino`](../firmware/reed_contact/reed_contact.ino)

> **Two firmware choices, same hardware.** This guide covers the Arduino firmware (options A and B). If you already run your devices with ESPHome, you can flash [`esphome/reed-contact.yaml`](../esphome/reed-contact.yaml) instead: see [Option C: ESPHome](#option-c-esphome) for how it differs and how to flash it. The wiring is identical for both.

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

## Option C: ESPHome

If you already manage your devices with [ESPHome](https://esphome.io/), you can flash [`esphome/reed-contact.yaml`](../esphome/reed-contact.yaml) instead of the sketch. It uses ESPHome's native Zigbee support for the ESP32-C6, so it is still a Zigbee router with no WiFi. **The wiring is exactly the same**: reed on D2 / GPIO2 to GND, external antenna through GPIO3 / GPIO14, BOOT button on GPIO9, LED on GPIO15.

### Arduino firmware or ESPHome?

| | Arduino firmware (options A, B) | ESPHome (option C) |
|---|---|---|
| Zigbee cluster | **IAS Zone** contact, the standard for door/window sensors | **Binary Input**: ESPHome has no IAS Zone support yet |
| Zigbee2MQTT converter | [`reed_contact.mjs`](../zigbee2mqtt/reed_contact.mjs) | [`reed_contact_esphome.mjs`](../zigbee2mqtt/reed_contact_esphome.mjs) |
| Result in Z2M and Home Assistant | `contact`, `binary_sensor` with device class *door* | the same |
| ZHA and other coordinators | Recognised as a contact sensor | Seen as a generic binary input |
| Model string | `C6ReedContact` | `C6ReedContactESPHome` |
| Browser flasher and release binaries | Yes | No, you build it with ESPHome |
| Updates | USB | USB. Over Zigbee only with the experimental OTA build (see below) |
| ESPHome version | | `dev` branch (2026.11 or newer) |
| Customising | Edit the C++ sketch | Edit YAML: a second reed, a relay or a temperature probe are a few lines |

Pick the Arduino firmware if you want the standard contact sensor and one-click flashing. Pick ESPHome if you prefer YAML and already have an ESPHome workflow.

### What the YAML does

Everything the sketch does, minus the IAS Zone enrollment, which a Binary Input doesn't need:

- Zigbee **router**, mains powered, manufacturer `DIY`, model `C6ReedContactESPHome`.
- Switches to the external U.FL antenna at boot.
- Reads the reed on **GPIO2** with the internal pull-up and a **200 ms** debounce. `presentValue` is `true` when the reed is open (magnet away or wire cut) and `false` when closed, so a failure still reads as *open*.
- Re-sends the state **every 5 minutes** as a heartbeat.
- **LED:** slow blink while looking for a network, off once joined.
- **Factory reset:** hold **BOOT** for 3 s, **then release it**. The board leaves the network and reboots ready to pair. The Arduino firmware resets while you are still holding the button; ESPHome waits for the release.

### Build and flash

1. Install ESPHome from the `dev` branch. Native Zigbee and the `manufacturer` option are not in a stable release yet:

   ```bash
   uv tool install https://github.com/esphome/esphome/archive/dev.zip
   ```

   Or `pip install https://github.com/esphome/esphome/archive/dev.zip` in a virtualenv, or the `ghcr.io/esphome/esphome:dev` Docker image.
2. Connect the XIAO over USB-C (with **no** 12 V on the buck). If it ran the Arduino firmware before, erase it first so no old Zigbee network data is left behind:

   ```bash
   esptool --chip esp32c6 erase-flash
   ```

3. Build and upload (the first run downloads the ESP-IDF toolchain):

   ```bash
   esphome run esphome/reed-contact.yaml
   ```

   Pick the USB serial port when asked. If the port isn't found: hold **BOOT**, tap **RESET**, release **BOOT**, then try again.
4. Watch the logs with `esphome logs esphome/reed-contact.yaml`.
5. Install [`zigbee2mqtt/reed_contact_esphome.mjs`](../zigbee2mqtt/reed_contact_esphome.mjs) as the Z2M converter, then pair as described in [the next guide](04-zigbee2mqtt-and-home-assistant.md).

### Settings you might change

| Arduino define | In the YAML |
|---|---|
| `REED_PIN` | `binary_sensor` → `contact` → `pin` → `number` |
| `DEBOUNCE_MS` | `filters` → `delayed_on_off` |
| `HEARTBEAT_MS` | the `5min` entry under `interval` |
| `FACTORY_RESET_HOLD_MS` | `on_click` → `min_length` |
| `useExternalAntenna()` | the `on_boot` block (remove it to use the ceramic antenna) |

### Updating an ESPHome build

ESPHome's usual OTA works over WiFi, which this build doesn't use. Unplug the 12 V, connect USB and run `esphome run` again **without erasing**: the board keeps its network keys and reconnects without re-pairing.

Updates over Zigbee are possible with an experimental `ota: platform: zigbee` component, in review at [mmornati/esphome#1](https://github.com/mmornati/esphome/pull/1) and not part of ESPHome yet. With it, Zigbee2MQTT pushes the new firmware over the mesh. It is slow (about 30 minutes for 360 KB), so plan for it.

### Switching between the two builds

The model string changes, so Z2M sees a new device. Remove it in Z2M, flash the other build with a full erase, pair again and give it the same friendly name (`garage_door_sensor`). Home Assistant keeps the same entity IDs.
