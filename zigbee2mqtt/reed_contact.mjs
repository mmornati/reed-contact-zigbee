// Zigbee2MQTT external converter for the DIY XIAO ESP32-C6 reed contact sensor.
// Works for anything a reed switch + magnet can watch: garage doors, gates, doors,
// windows, sheds, mailboxes, cabinets...
// Copy to <z2m data dir>/external_converters/reed_contact.mjs and restart Zigbee2MQTT.
import * as m from 'zigbee-herdsman-converters/lib/modernExtend';

export default {
    zigbeeModel: ['C6ReedContact'],
    model: 'C6ReedContact',
    vendor: 'DIY',
    description: 'Mains-powered reed contact sensor and Zigbee router (XIAO ESP32-C6)',
    // contact: true = magnet in front of the reed (closed), false = open
    extend: [m.iasZoneAlarm({zoneType: 'contact', zoneAttributes: ['alarm_1']})],
};
