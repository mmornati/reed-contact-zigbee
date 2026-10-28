// Host-side unit tests for ContactReporter (run with: pio test -e native)
#include <unity.h>

#include "contact_logic.h"

static const uint32_t DEBOUNCE = 200;
static const uint32_t HEARTBEAT = 300000;

void setUp() {}
void tearDown() {}

void test_begin_sets_initial_state() {
  ContactReporter closedAtBoot(DEBOUNCE, HEARTBEAT);
  closedAtBoot.begin(true, 0);
  TEST_ASSERT_TRUE(closedAtBoot.closed());

  ContactReporter openAtBoot(DEBOUNCE, HEARTBEAT);
  openAtBoot.begin(false, 0);
  TEST_ASSERT_FALSE(openAtBoot.closed());
}

void test_stable_state_reports_nothing() {
  ContactReporter r(DEBOUNCE, HEARTBEAT);
  r.begin(true, 0);
  for (uint32_t t = 0; t < 10000; t += 20) {
    TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(true, t));
  }
}

void test_change_reported_only_after_debounce() {
  ContactReporter r(DEBOUNCE, HEARTBEAT);
  r.begin(true, 0);

  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(false, 1000));
  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(false, 1199));
  TEST_ASSERT_TRUE(r.closed());

  TEST_ASSERT_EQUAL(ContactReporter::REPORT_CHANGE, r.update(false, 1200));
  TEST_ASSERT_FALSE(r.closed());

  // Reported once, not again while it stays open.
  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(false, 1220));
}

void test_bounce_shorter_than_debounce_is_ignored() {
  ContactReporter r(DEBOUNCE, HEARTBEAT);
  r.begin(true, 0);

  // Contact chatters (motor vibration, magnet passing by) and settles back closed.
  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(false, 1000));
  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(true, 1050));
  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(false, 1100));
  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(true, 1150));
  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(true, 2000));
  TEST_ASSERT_TRUE(r.closed());
}

void test_bouncing_restarts_debounce_window() {
  ContactReporter r(DEBOUNCE, HEARTBEAT);
  r.begin(true, 0);

  r.update(false, 1000);
  r.update(true, 1150);   // bounce back
  r.update(false, 1180);  // open again: window restarts here
  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(false, 1379));
  TEST_ASSERT_EQUAL(ContactReporter::REPORT_CHANGE, r.update(false, 1380));
}

void test_open_then_close_reports_both() {
  ContactReporter r(DEBOUNCE, HEARTBEAT);
  r.begin(true, 0);

  r.update(false, 1000);
  TEST_ASSERT_EQUAL(ContactReporter::REPORT_CHANGE, r.update(false, 1200));
  r.update(true, 5000);
  TEST_ASSERT_EQUAL(ContactReporter::REPORT_CHANGE, r.update(true, 5200));
  TEST_ASSERT_TRUE(r.closed());
}

void test_heartbeat_resends_state() {
  ContactReporter r(DEBOUNCE, HEARTBEAT);
  r.begin(false, 0);

  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(false, HEARTBEAT - 1));
  TEST_ASSERT_EQUAL(ContactReporter::REPORT_HEARTBEAT, r.update(false, HEARTBEAT));
  TEST_ASSERT_FALSE(r.closed());
  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(false, HEARTBEAT + 1));
  TEST_ASSERT_EQUAL(ContactReporter::REPORT_HEARTBEAT, r.update(false, 2 * HEARTBEAT));
}

void test_change_resets_heartbeat_timer() {
  ContactReporter r(DEBOUNCE, HEARTBEAT);
  r.begin(true, 0);

  r.update(false, 100000);
  TEST_ASSERT_EQUAL(ContactReporter::REPORT_CHANGE, r.update(false, 100200));
  // Next heartbeat is counted from the change, not from boot.
  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(false, HEARTBEAT + 1000));
  TEST_ASSERT_EQUAL(ContactReporter::REPORT_HEARTBEAT, r.update(false, 100200 + HEARTBEAT));
}

void test_millis_rollover() {
  ContactReporter r(DEBOUNCE, HEARTBEAT);
  const uint32_t nearWrap = 0xFFFFFFFFu - 50;
  r.begin(true, nearWrap);

  r.update(false, nearWrap + 10);
  TEST_ASSERT_EQUAL(ContactReporter::NONE, r.update(false, 100));  // wrapped, 141 ms later
  TEST_ASSERT_EQUAL(ContactReporter::REPORT_CHANGE, r.update(false, 159));  // 200 ms later
  TEST_ASSERT_FALSE(r.closed());
}

int main(int, char **) {
  UNITY_BEGIN();
  RUN_TEST(test_begin_sets_initial_state);
  RUN_TEST(test_stable_state_reports_nothing);
  RUN_TEST(test_change_reported_only_after_debounce);
  RUN_TEST(test_bounce_shorter_than_debounce_is_ignored);
  RUN_TEST(test_bouncing_restarts_debounce_window);
  RUN_TEST(test_open_then_close_reports_both);
  RUN_TEST(test_heartbeat_resends_state);
  RUN_TEST(test_change_resets_heartbeat_timer);
  RUN_TEST(test_millis_rollover);
  return UNITY_END();
}
