#include <stdio.h>

#include "nrf_ble_scan.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"

#define APP_BLE_CONN_CFG_TAG 1
#define APP_BLE_OBSERVER_PRIO 3

NRF_BLE_SCAN_DEF(m_scan);

void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name) {
  app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

static void scan_start(void) {
  ret_code_t err_code = nrf_ble_scan_start(&m_scan);
  APP_ERROR_CHECK(err_code);
}

static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context) {
  ret_code_t err_code;

  ble_gap_evt_t const *p_gap_evt = &p_ble_evt->evt.gap_evt;

  switch (p_ble_evt->header.evt_id) {
  case BLE_GAP_EVT_ADV_REPORT: {
    NRF_LOG_RAW_HEXDUMP_INFO(m_scan.scan_buffer.p_data, m_scan.scan_buffer.len);
    NRF_LOG_RAW_INFO("----------------------------------\r\n");
  } break;

  default:
    break;
  }
}

static void ble_stack_init(void) {
  ret_code_t err_code;

  err_code = nrf_sdh_enable_request();
  APP_ERROR_CHECK(err_code);

  uint32_t ram_start = 0;
  err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_sdh_ble_enable(&ram_start);
  APP_ERROR_CHECK(err_code);

  NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

static void scan_evt_handler(scan_evt_t const *p_scan_evt) {
  ret_code_t err_code;

  switch (p_scan_evt->scan_evt_id) {
  default:
    break;
  }
}

static void log_init(void) {
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();
}

static void power_management_init(void) {
  ret_code_t err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
}

static void scan_init(void) {
  ret_code_t err_code;
  nrf_ble_scan_init_t init_scan;

  memset(&init_scan, 0, sizeof(init_scan));

  init_scan.connect_if_match = true;
  init_scan.conn_cfg_tag = APP_BLE_CONN_CFG_TAG;

  err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
  APP_ERROR_CHECK(err_code);
}

static void idle_state_handle(void) {
  NRF_LOG_FLUSH();
  nrf_pwr_mgmt_run();
}

int main(void) {
  log_init();
  power_management_init();
  ble_stack_init();
  scan_init();

  NRF_LOG_INFO("Blinky CENTRAL example started.");
  scan_start();

  for (;;) {
    idle_state_handle();
  }
}
