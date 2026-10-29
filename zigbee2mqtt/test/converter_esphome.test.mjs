// Integration tests for the ESPHome build's converter, loaded through
// zigbee-herdsman-converters like Zigbee2MQTT does.
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import * as zhc from 'zigbee-herdsman-converters';

import converter from '../reed_contact_esphome.mjs';
import arduinoConverter from '../reed_contact.mjs';

const yaml = readFileSync(new URL('../../esphome/reed-contact.yaml', import.meta.url), 'utf8');
const definition = zhc.prepareDefinition({...converter});

function yamlValue(key) {
    const match = yaml.match(new RegExp(`^\\s+${key}:\\s*(\\S+)`, 'm'));
    assert.ok(match, `${key} not found in esphome/reed-contact.yaml`);
    return match[1];
}

async function decode(type, data) {
    const device = {modelID: converter.model, manufacturerName: converter.vendor};
    const meta = {device, logger: console, state: {}};
    const msg = {type, cluster: 'genBinaryInput', data, endpoint: {ID: 1}, device};
    const result = {};
    for (const fz of definition.fromZigbee) {
        const types = Array.isArray(fz.type) ? fz.type : [fz.type];
        if (fz.cluster === 'genBinaryInput' && types.includes(type)) {
            Object.assign(result, await fz.convert(definition, msg, () => {}, {}, meta));
        }
    }
    return result;
}

test('ESPHome YAML still sends the expected manufacturer and model', () => {
    assert.equal(yamlValue('manufacturer'), converter.vendor);
    assert.equal(yamlValue('model'), converter.model);
    assert.deepEqual(converter.zigbeeModel, [converter.model]);
});

test('does not clash with the Arduino firmware converter', () => {
    assert.notEqual(converter.model, arduinoConverter.model);
});

test('Zigbee2MQTT resolves a device reporting the ESPHome model', async () => {
    zhc.addExternalDefinition({...converter, externalConverterName: 'reed_contact_esphome.mjs'});
    try {
        const device = {
            ieeeAddr: '0x0000000000000002',
            modelID: converter.model,
            manufacturerName: converter.vendor,
            type: 'Router',
            powerSource: 'Mains (single phase)',
            endpoints: [],
            interviewing: false,
            interviewCompleted: true,
            getEndpoint: () => undefined,
        };
        const found = await zhc.findByDevice(device);
        assert.ok(found, 'definition not found');
        assert.equal(found.model, converter.model);
    } finally {
        zhc.removeExternalDefinitions('reed_contact_esphome.mjs');
    }
});

test('exposes a single binary "contact" (true = closed)', () => {
    const list = definition.exposes.filter((e) => e.name !== 'linkquality');
    assert.equal(list.length, 1);
    const [contact] = list;
    assert.equal(contact.type, 'binary');
    assert.equal(contact.property, 'contact');
    assert.equal(contact.value_on, false, 'Home Assistant "on" (door open) must be contact=false');
});

for (const type of ['attributeReport', 'readResponse']) {
    test(`${type}: closed reed (presentValue false) -> contact true`, async () => {
        assert.equal((await decode(type, {presentValue: 0})).contact, true);
    });

    test(`${type}: open reed (presentValue true) -> contact false`, async () => {
        assert.equal((await decode(type, {presentValue: 1})).contact, false);
    });
}
