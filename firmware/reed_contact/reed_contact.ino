/**
 * Reed contact sensor — Seeed XIAO ESP32-C6, mains powered, Zigbee router.
 * Watches anything a reed switch + magnet can: garage door, gate, door, window...
 *
 * Reports one reed switch as a Zigbee IAS Zone contact sensor (zone type: contact).
 *   reed closed (magnet in front)        -> GPIO LOW  -> setClosed()
 *   reed open   (magnet away / wire cut) -> GPIO HIGH -> setOpen()
 * A cut wire therefore reads as "open": the failure is visible, never silent.
 *
 * PlatformIO: all settings are in ../platformio.ini.
 * Arduino IDE, Tools menu:
 *   Board:            XIAO_ESP32C6
 *   Zigbee mode:      Zigbee ZCZR (coordinator/router)
 *   Partition Scheme: Zigbee ZCZR 4MB with spiffs
 *   Erase All Flash Before Sketch Upload: Enabled (first flash only)
 *
 * Requires arduino-esp32 core 3.3.0 or newer (IAS Zone enroll API).
 */

#include <Arduino.h>
#ifndef ZIGBEE_MODE_ZCZR
#error "Select Tools -> Zigbee mode -> Zigbee ZCZR (coordinator/router)"
#endif

#include "Zigbee.h"
#include <Preferences.h>
#include "contact_logic.h"

/* ---------- Pins (XIAO ESP32-C6) ---------- */
#define REED_PIN        2    // D2 = GPIO2, reed switch to GND
#define LED_PIN         15   // user LED, active LOW
#define BUTTON_PIN      BOOT_PIN  // GPIO9, BOOT button
#define RF_SWITCH_EN    3    // LOW = enable RF switch control
#define RF_ANT_SELECT   14   // HIGH = external U.FL antenna, LOW = ceramic

/* ---------- Behaviour ---------- */
#define ENDPOINT                 1
#define DEBOUNCE_MS              200      // reed must be stable this long
#define HEARTBEAT_MS             (5UL * 60UL * 1000UL)  // re-send state every 5 min
#define ENROLL_TIMEOUT_MS        30000
#define ENROLL_RETRY_MS          60000
#define FACTORY_RESET_HOLD_MS    3000

ZigbeeContactSwitch zbContact = ZigbeeContactSwitch(ENDPOINT);
ContactReporter reporter(DEBOUNCE_MS, HEARTBEAT_MS);
Preferences preferences;

unsigned long lastEnrollAttemptMs = 0;

static inline void ledOn()  { digitalWrite(LED_PIN, LOW); }
static inline void ledOff() { digitalWrite(LED_PIN, HIGH); }

static inline bool readReedClosed() {
  return digitalRead(REED_PIN) == LOW;
}

void useExternalAntenna() {
  pinMode(RF_SWITCH_EN, OUTPUT);
  digitalWrite(RF_SWITCH_EN, LOW);
  delay(100);
  pinMode(RF_ANT_SELECT, OUTPUT);
  digitalWrite(RF_ANT_SELECT, HIGH);
}

bool loadEnrolledFlag() {
  preferences.begin("reedcontact", true);
  bool enrolled = preferences.getBool("enrolled", false);
  preferences.end();
  return enrolled;
}

void saveEnrolledFlag(bool enrolled) {
  preferences.begin("reedcontact", false);
  preferences.putBool("enrolled", enrolled);
  preferences.end();
}

void sendState(bool closed) {
  if (closed) {
    zbContact.setClosed();
  } else {
    zbContact.setOpen();
  }
  Serial.printf("Contact %s\n", closed ? "CLOSED" : "OPEN");
}

void checkFactoryReset() {
  if (digitalRead(BUTTON_PIN) != LOW) {
    return;
  }
  unsigned long start = millis();
  while (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);
    // Fast blink while held so you know when to release.
    digitalWrite(LED_PIN, (millis() / 100) % 2);
    if (millis() - start > FACTORY_RESET_HOLD_MS) {
      Serial.println("Factory reset: leaving network and rebooting");
      saveEnrolledFlag(false);
      ledOn();
      delay(1000);
      Zigbee.factoryReset();  // reboots
    }
  }
  ledOff();
}

void tryEnroll(bool restore) {
  lastEnrollAttemptMs = millis();
  if (restore) {
    Serial.println("Restoring IAS Zone enrollment");
    zbContact.restoreIASZoneEnroll();
  } else {
    Serial.println("Requesting IAS Zone enrollment");
    zbContact.requestIASZoneEnroll();
  }

  unsigned long start = millis();
  while (!zbContact.enrolled() && millis() - start < ENROLL_TIMEOUT_MS) {
    delay(100);
  }

  if (zbContact.enrolled()) {
    Serial.println("Enrolled");
    if (!restore) {
      saveEnrolledFlag(true);
    }
  } else {
    Serial.println("Enrollment timed out, will retry");
  }
}

void setup() {
  Serial.begin(115200);

  useExternalAntenna();

  pinMode(LED_PIN, OUTPUT);
  ledOff();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(REED_PIN, INPUT_PULLUP);  // an external 10k pull-up is still recommended

  zbContact.setManufacturerAndModel("DIY", "C6ReedContact");
  zbContact.setPowerSource(ZB_POWER_SOURCE_MAINS);
  Zigbee.addEndpoint(&zbContact);

  Serial.println("Starting Zigbee (router)");
  if (!Zigbee.begin(ZIGBEE_ROUTER)) {
    Serial.println("Zigbee failed to start, rebooting");
    delay(1000);
    ESP.restart();
  }

  // Slow blink until joined. Open "Permit join" in Zigbee2MQTT now.
  Serial.print("Joining network");
  while (!Zigbee.connected()) {
    digitalWrite(LED_PIN, (millis() / 500) % 2);
    checkFactoryReset();
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  ledOff();

  tryEnroll(loadEnrolledFlag());

  reporter.begin(readReedClosed(), millis());
  sendState(reporter.closed());
}

void loop() {
  unsigned long now = millis();

  if (reporter.update(readReedClosed(), now) != ContactReporter::NONE) {
    sendState(reporter.closed());
  }

  if (!zbContact.enrolled() && now - lastEnrollAttemptMs >= ENROLL_RETRY_MS) {
    tryEnroll(false);
  }

  checkFactoryReset();
  delay(20);
}
