/* A simple BLE scanner example - prints out raw data from scanned advertisements */

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

// A macro for defining a nrf_ble_scan instance, with m_scan as the name of the instance
NRF_BLE_SCAN_DEF(m_scan);

void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name) {
  app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

static void scan_start(void) {

/* nrf_ble_scan_start: part of the BLE library Scanning Module
** takes  a pointer to a scanning module instance
** can take a pointer to GAP scanning parameters, possibly NULL
*/
  ret_code_t err_code = nrf_ble_scan_start(&m_scan);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_INFO("scan_start SUCCESSFUL");
}

static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context) {
  switch (p_ble_evt->header.evt_id) {
  case BLE_GAP_EVT_ADV_REPORT: {
    NRF_LOG_RAW_HEXDUMP_INFO(m_scan.scan_buffer.p_data, m_scan.scan_buffer.len);
    NRF_LOG_RAW_INFO("# # # # # # # # # # # # # # # # \r\n");
  } break;
  default:
    break;
  }
}

static void ble_stack_init(void) {
  ret_code_t err_code;

/* nrf_sdh_enable_request: part of SDK Common Libraries, SoftDevice Handler
** Function to request enabling of SoftDevice by issuing an NRF_SDH_EVT_ENABLE_REQUEST
** to all observers registered using the NRF_SDH_REQUEST_OBSERVER macro.  If all observers
** acknowledge the request, the SoftDevice is enabled
*/
  err_code = nrf_sdh_enable_request();
  APP_ERROR_CHECK(err_code);


/* nrf_sdh_ble_default_cfg_set: BLET Support in SoftDevice Handler
** Sets the default BLE stack configuration, configuring 
** number of peripheral links
** number of central links
** ATT MTU size
** Vendor specific UUID count
** GATTS Attribute table size
** Service changed
*/
  uint32_t ram_start = 0;
  err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
  APP_ERROR_CHECK(err_code);

/* nrf_sdh_ble_enable: function for retrieving the address of the start of the application's RAM */
  err_code = nrf_sdh_ble_enable(&ram_start);
  APP_ERROR_CHECK(err_code);

/* NRF_SDH_BLE_OBSERVER: BLE Support for SoftDevice
** Macro for registering a nrf_sdh_ble_evet_observer_t modules to be notified about SoC events.
** arguments are  observer name, priority of the observer event handler, BLE event handler, and parameter to handler
*/
  NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
  NRF_LOG_INFO("ble_stack_init SUCCESSFUL");
}

static void log_init(void) {

/* NRF_LOG_INIT: Logger Module
** Macro for initializing the logs.
** first parameter is the timestamp function (nrf_log_timestamp_func_t
** second parameter is optional timestamp frequency in Hz; otherwise default is used
*/
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

/* NRF_LOG_DEFAULT_BACKENDS_INIT - Logger Module
** Macro for initializingg default backends
*/
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  NRF_LOG_INFO("log_init SUCCESSFUL");
}

static void power_management_init(void) {

/* nrf_pwr_mgmt_init: SDK Common Libraries, Power Management, Function for initliazing power management
** This operatoin is unsafe with the SoftDevice from interrupt priority higher than SVC
*/ 
  ret_code_t err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
  NRF_LOG_INFO("power_management_init SUCCESSFUL");
}

static void scan_init(void) {
  ret_code_t err_code;

/* nrf_ble_scan_init: Scanning Module function to initiliaze the scanning module
** takes pointer to nrf_scan_ble_t used to identify instance
** point of parameters to initialize module, may be null
** handler for scanning events - can be NULL if no handling is implemented
*/
  err_code = nrf_ble_scan_init(&m_scan, NULL, NULL);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_INFO("scan_init SUCCESSFUL");
}

static void idle_state_handle(void) {

/* NRF_LOG_FLUSH - Macro for flushing log data before reset
*/ 
  NRF_LOG_FLUSH();
/* nmrf_pwr_mgmt_run: Function for running power management; should be run in main loop */
  nrf_pwr_mgmt_run();
}

int main(void) {
  log_init();
  NRF_LOG_INFO("Blinky CENTRAL example started.");
  power_management_init();
  ble_stack_init();
  scan_init();
  scan_start();
  for (;;) {
    idle_state_handle();
  }
}
