# 4. Zigbee2MQTT and Home Assistant

🌐 **English** · [Français](fr/04-zigbee2mqtt-and-home-assistant.md) · [Italiano](it/04-zigbee2mqtt-and-home-assistant.md)

## 4.1 Install the external converter (Zigbee2MQTT 2.x)

Z2M doesn't know a DIY device, so a tiny converter tells it "this is a contact sensor".

1. Copy [`zigbee2mqtt/reed_contact.mjs`](../zigbee2mqtt/reed_contact.mjs) into the Z2M data folder, under `external_converters/`:
   - HA add-on: `/config/zigbee2mqtt/external_converters/reed_contact.mjs` (use the File editor or Samba add-on)
   - Docker: `<your data volume>/external_converters/reed_contact.mjs`
2. Restart Zigbee2MQTT. In Z2M 2.x, everything in `external_converters/` loads automatically, with no `configuration.yaml` change.
3. Check the Z2M log for a line saying the external converter `reed_contact.mjs` was loaded.

## 4.2 Pair the sensor

1. In the Z2M UI, click **Permit join (All)**. Picking the coordinator or a router near the garage helps.
2. Power the XIAO (USB on the bench, or 12 V once installed). The LED blinks slowly, then turns off once joined.
3. Z2M interviews it and should show **DIY C6ReedContact, "Mains-powered reed contact sensor and Zigbee router (XIAO ESP32-C6)"**, type **Router**.
4. Rename it to **`garage_door_sensor`** and tick *Update Home Assistant entity ID*.
5. Check the *Exposes* tab: `contact` should be **true** with the magnet in front and **false** without it.

If it shows as "Unsupported", the converter isn't loaded or the model string doesn't match. See [Troubleshooting](05-troubleshooting.md).

### Recommended Z2M settings

- *Settings → Availability* on. Routers get pinged actively, so HA sees `unavailable` if the sensor dies.
- The ZB-SW01 should be named **`garage_opener`**, which gives `switch.garage_opener`.

## 4.3 Entities in Home Assistant

Through MQTT discovery you get:

| Entity | Meaning |
|---|---|
| `binary_sensor.garage_door_sensor_contact` | device class *door*: **on = not fully closed**, **off = closed** |
| `sensor.garage_door_sensor_linkquality` | Zigbee LQI (disabled by default) |
| `switch.garage_opener` | ZB-SW01 relay |

> Check the real entity IDs in *Settings → Devices → garage_door_sensor*. If they differ, search and replace them in the package below.

## 4.4 Install the HA package

[`homeassistant/garage_door.yaml`](../homeassistant/garage_door.yaml) contains:

| Piece | What it does |
|---|---|
| `script.garage_door_pulse` | Pulses the ZB-SW01 for 800 ms, like pressing the wall button |
| `script.garage_door_open` | Pulses only if the door is closed |
| `script.garage_door_close` | Pulses only if the door is open, then **checks that it is fully closed within 90 s**, and notifies you if not |
| `cover.garage_door` | Template cover (device class *garage*) with open/close buttons and the sensor state |
| `input_number.garage_door_open_alert_minutes` | Delay before the "left open" alert (default 10 min) |
| Automation *left open* | Notifies when the door has been not fully closed for N minutes |
| Automation *night check* | Notifies at 22:00 if the door isn't closed |
| Automation *sensor offline* | Notifies if the sensor has been unavailable for 15 min |

Steps:

1. In `configuration.yaml`, if it isn't there yet:

   ```yaml
   homeassistant:
     packages: !include_dir_named packages
   ```

2. Copy the file to `/config/packages/garage_door.yaml`.
3. Replace `notify.notify` with your notifier, e.g. `notify.mobile_app_<your_phone>` or your ntfy notifier.
4. *Developer tools → YAML → Check configuration*, then **Restart**. Template covers and new packages need a restart, not just a reload.
5. Add `cover.garage_door` to a dashboard. A *Tile* card with the cover open/close feature works well.

### Why the cover has no "stop"

The ZB-SW01 has a single button: pulse = open / stop / close, depending on the motor. With only a "closed" sensor, HA can't know whether the door is moving, so a stop button would behave unpredictably. The open/close scripts only pulse when it's safe: open only when closed, close only when not closed.

### The "not completely closed" check

This is the main goal of the project. It's covered three ways:

1. **Fully closed** is the only state that reads `off`. Any gap of a few centimetres reads `on` (not closed).
2. After a close requested from HA, `script.garage_door_close` waits 90 s and notifies you if the door still isn't shut. That catches an obstacle, the motor's safety reverse, or a door stopped halfway.
3. Whatever opened the door (keypad, remote, wall button, HA), the *left open* automation notifies you after N minutes, and the 22:00 check is a last reminder.

## 4.5 Ideas for later

- An **actionable notification** on the "left open" alert with a *Close now* button that calls `script.garage_door_close`.
- Auto-close at night, only if a motion sensor says no one is in the garage.
- A second reed at the fully-open position (one spare in the kit) to get open / closed / opening / closing states. That needs a second endpoint in the firmware.
