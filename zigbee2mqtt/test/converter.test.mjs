// Integration tests: load the external converter through zigbee-herdsman-converters,
// the same library Zigbee2MQTT uses, and check what Z2M will see.
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import * as zhc from 'zigbee-herdsman-converters';

import converter from '../reed_contact.mjs';

// Must match setManufacturerAndModel() in firmware/reed_contact/reed_contact.ino
const FIRMWARE_MANUFACTURER = 'DIY';
const FIRMWARE_MODEL = 'C6ReedContact';

// IAS Zone status bitmap: bit 0 = alarm1 (set by setOpen(), cleared by setClosed())
const ZONE_CLOSED = 0b0;
const ZONE_OPEN = 0b1;

const definition = zhc.prepareDefinition({...converter});

function exposes() {
    const list = typeof definition.exposes === 'function' ? definition.exposes(undefined, {}) : definition.exposes;
    return list.filter((e) => e.name !== 'linkquality');
}

async function decode(type, data) {
    const device = {modelID: FIRMWARE_MODEL, manufacturerName: FIRMWARE_MANUFACTURER};
    const meta = {device, logger: console, state: {}};
    const msg = {type, cluster: 'ssIasZone', data, endpoint: {ID: 1}, device};
    const result = {};
    for (const fz of definition.fromZigbee) {
        const types = Array.isArray(fz.type) ? fz.type : [fz.type];
        if (fz.cluster === 'ssIasZone' && types.includes(type)) {
            Object.assign(result, await fz.convert(definition, msg, () => {}, {}, meta));
        }
    }
    return result;
}

test('firmware source still sends the expected manufacturer and model', () => {
    const sketch = readFileSync(new URL('../../firmware/reed_contact/reed_contact.ino', import.meta.url), 'utf8');
    const match = sketch.match(/setManufacturerAndModel\("([^"]+)",\s*"([^"]+)"\)/);
    assert.ok(match, 'setManufacturerAndModel() call not found in the sketch');
    assert.equal(match[1], FIRMWARE_MANUFACTURER);
    assert.equal(match[2], FIRMWARE_MODEL);
});

test('matches the model string sent by the firmware', () => {
    assert.deepEqual(converter.zigbeeModel, [FIRMWARE_MODEL]);
    assert.equal(converter.model, FIRMWARE_MODEL);
    assert.equal(converter.vendor, FIRMWARE_MANUFACTURER);
});

test('Zigbee2MQTT resolves a device reporting the firmware model', async () => {
    zhc.addExternalDefinition({...converter, externalConverterName: 'reed_contact.mjs'});
    try {
        const device = {
            ieeeAddr: '0x0000000000000001',
            modelID: FIRMWARE_MODEL,
            manufacturerName: FIRMWARE_MANUFACTURER,
            type: 'Router',
            powerSource: 'Mains (single phase)',
            endpoints: [],
            interviewing: false,
            interviewCompleted: true,
            getEndpoint: () => undefined,
        };
        const found = await zhc.findByDevice(device);
        assert.ok(found, 'definition not found');
        assert.equal(found.model, FIRMWARE_MODEL);
        assert.equal(found.vendor, FIRMWARE_MANUFACTURER);
    } finally {
        zhc.removeExternalDefinitions('reed_contact.mjs');
    }
});

test('exposes a single binary "contact" (true = closed)', () => {
    const list = exposes();
    assert.equal(list.length, 1);
    const [contact] = list;
    assert.equal(contact.type, 'binary');
    assert.equal(contact.property, 'contact');
    assert.equal(contact.value_on, false, 'Home Assistant "on" (door open) must be contact=false');
    assert.equal(contact.value_off, true);
});

for (const type of ['commandStatusChangeNotification', 'attributeReport', 'readResponse']) {
    test(`${type}: closed reed -> contact true`, async () => {
        const data = type === 'commandStatusChangeNotification' ? {zonestatus: ZONE_CLOSED, extendedstatus: 0} : {zoneStatus: ZONE_CLOSED};
        assert.equal((await decode(type, data)).contact, true);
    });

    test(`${type}: open reed -> contact false`, async () => {
        const data = type === 'commandStatusChangeNotification' ? {zonestatus: ZONE_OPEN, extendedstatus: 0} : {zoneStatus: ZONE_OPEN};
        assert.equal((await decode(type, data)).contact, false);
    });
}
