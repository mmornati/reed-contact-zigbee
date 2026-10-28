#pragma once

#include <stdint.h>

/**
 * Decides when the contact state has to be sent to the Zigbee network.
 *
 * - A change is reported only once the raw reading has been stable for `debounceMs`.
 * - The current state is re-sent every `heartbeatMs`, so a lost message never leaves
 *   the coordinator with a stale value.
 *
 * Pure logic with no Arduino dependency, so it is unit tested on the host
 * (see firmware/test). Time arithmetic is unsigned, which keeps it correct when
 * millis() wraps around after ~49 days.
 */
class ContactReporter {
public:
  enum Action : uint8_t {
    NONE = 0,
    REPORT_CHANGE,
    REPORT_HEARTBEAT,
  };

  ContactReporter(uint32_t debounceMs, uint32_t heartbeatMs)
    : _debounceMs(debounceMs), _heartbeatMs(heartbeatMs) {}

  // Sets the initial state. The caller sends it right away.
  void begin(bool closed, uint32_t nowMs) {
    _reportedClosed = closed;
    _lastRawClosed = closed;
    _lastRawChangeMs = nowMs;
    _lastReportMs = nowMs;
  }

  Action update(bool rawClosed, uint32_t nowMs) {
    if (rawClosed != _lastRawClosed) {
      _lastRawClosed = rawClosed;
      _lastRawChangeMs = nowMs;
    }

    if (rawClosed != _reportedClosed && nowMs - _lastRawChangeMs >= _debounceMs) {
      _reportedClosed = rawClosed;
      _lastReportMs = nowMs;
      return REPORT_CHANGE;
    }

    if (nowMs - _lastReportMs >= _heartbeatMs) {
      _lastReportMs = nowMs;
      return REPORT_HEARTBEAT;
    }

    return NONE;
  }

  // Last state reported to the network.
  bool closed() const {
    return _reportedClosed;
  }

private:
  uint32_t _debounceMs;
  uint32_t _heartbeatMs;
  bool _reportedClosed = false;
  bool _lastRawClosed = false;
  uint32_t _lastRawChangeMs = 0;
  uint32_t _lastReportMs = 0;
};
