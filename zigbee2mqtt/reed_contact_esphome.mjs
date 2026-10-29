// Zigbee2MQTT external converter for the ESPHome build of the reed contact sensor
// (esphome/reed-contact.yaml). ESPHome reports the reed as a Binary Input instead of
// an IAS Zone, so it needs its own converter.
// Copy to <z2m data dir>/external_converters/reed_contact_esphome.mjs and restart Zigbee2MQTT.
import * as exposes from 'zigbee-herdsman-converters/lib/exposes';
import * as reporting from 'zigbee-herdsman-converters/lib/reporting';

const e = exposes.presets;

// presentValue: true = open (reed HIGH), false = closed. Z2M contact: true = closed.
const fzContact = {
    cluster: 'genBinaryInput',
    type: ['attributeReport', 'readResponse'],
    convert: (model, msg) => {
        if (typeof msg.data.presentValue === 'undefined') return;
        return {contact: !msg.data.presentValue};
    },
};

export default {
    zigbeeModel: ['C6ReedContactESPHome'],
    model: 'C6ReedContactESPHome',
    vendor: 'DIY',
    description: 'Mains-powered reed contact sensor and Zigbee router (XIAO ESP32-C6, ESPHome)',
    fromZigbee: [fzContact],
    toZigbee: [],
    exposes: [e.contact()],
    configure: async (device, coordinatorEndpoint) => {
        const endpoint = device.getEndpoint(1);
        await reporting.bind(endpoint, coordinatorEndpoint, ['genBinaryInput']);
        await endpoint.read('genBinaryInput', ['presentValue']);
    },
};
