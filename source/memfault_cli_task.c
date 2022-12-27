//! @file
//!
//! @brief
//! This file contains task and functions related to handling IO and executing Memfault demo CLI
//! commands (https://mflt.io/demo-cli). These commands can be helpful for quickly experimenting
//! with and testing Memfault functionality

#include <FreeRTOS.h>
#include <task.h>

#include "ap.h"
#include "app_kvstore.h"
#include "cy_retarget_io.h"
#include "cyhal.h"
#include "cyhal_gpio.h"
#include "memfault/components.h"
#include "memfault_example_app.h"

#define MEMFAULT_CLI_TASK_SIZE (1024)
#define MEMFAULT_CLI_TASK_PRIORITY (1)
#define MAX_WIFI_CONN_RETRIES (5u)

// Helper functions to drive wifi commands
static int prv_join_wifi_cmd(int argc, char *argv[]);
static int prv_save_wifi_cmd(int argc, char *argv[]);
static int prv_scan_wifi_cmd(int argc, char *argv[]);

static const sMemfaultShellCommand s_memfault_shell_commands[] = {
  {"clear_core", memfault_demo_cli_cmd_clear_core, "Clear an existing coredump"},
  {"drain_chunks", memfault_demo_drain_chunk_data,
   "Flushes queued Memfault data. To upload data see https://mflt.io/posting-chunks-with-gdb"},
  {"export", memfault_demo_cli_cmd_export,
   "Export base64-encoded chunks. To upload data see https://mflt.io/chunk-data-export"},
  {"get_core", memfault_demo_cli_cmd_get_core, "Get coredump info"},
  {"get_device_info", memfault_demo_cli_cmd_get_device_info, "Get device info"},

  //
  // Test commands for validating SDK functionality: https://mflt.io/mcu-test-commands
  //

  {"test_assert", memfault_demo_cli_cmd_assert, "Trigger memfault assert"},

#if MEMFAULT_COMPILER_ARM
  {"test_busfault", memfault_demo_cli_cmd_busfault, "Trigger a busfault"},
  {"test_hardfault", memfault_demo_cli_cmd_hardfault, "Trigger a hardfault"},
  {"test_memmanage", memfault_demo_cli_cmd_memmanage, "Trigger a memory management fault"},
  {"test_usagefault", memfault_demo_cli_cmd_usagefault, "Trigger a usage fault"},
#endif

  {"test_log", memfault_demo_cli_cmd_test_log, "Writes test logs to log buffer"},
  {"test_log_capture", memfault_demo_cli_cmd_trigger_logs,
   "Trigger capture of current log buffer contents"},
  {"test_reboot", memfault_demo_cli_cmd_system_reboot,
   "Force system reset and track it with a trace event"},
  {"test_trace", memfault_demo_cli_cmd_trace_event_capture, "Capture an example trace event"},
  {"wifi_join", prv_join_wifi_cmd, "Join a WiFi network"},
  {"wifi_save", prv_save_wifi_cmd, "Save WiFi network info to auto-join at boot"},
  {"wifi_scan", prv_scan_wifi_cmd,
   "Scan available networks, reports network name and security type"},
  {"help", memfault_shell_help_handler, "Lists all commands"},
};

const sMemfaultShellCommand *const g_memfault_shell_commands = s_memfault_shell_commands;
const size_t g_memfault_num_shell_commands = MEMFAULT_ARRAY_SIZE(s_memfault_shell_commands);

// Joins a WiFi network
static int prv_join_wifi_cmd(int argc, char *argv[]) {
  if (argc < 4) {
    MEMFAULT_LOG_ERROR("Usage: wifi_join <SSID> <AUTH_TYPE> <PASSWORD>");
    return -1;
  }

  return connect_to_wifi_ap(argv[1], argv[2], argv[3], MAX_WIFI_CONN_RETRIES);
}

// Scans for available WiFi networks
static int prv_scan_wifi_cmd(int argc, char *argv[]) {
  MEMFAULT_LOG_INFO("#### Scan Results ####\n\n");
  MEMFAULT_LOG_INFO("SSID                 Security Type  RSSI(dBm)  Channel BSSID\n");

  return scan_wifi_ap();
}

// Saves WiFi network config to app kv-store
static int prv_save_wifi_cmd(int argc, char *argv[]) {
  if (argc < 4) {
    MEMFAULT_LOG_ERROR("Usage: wifi_save <SSID> <AUTH_TYPE> <PASSWORD>");
    return -1;
  }

  size_t len = strnlen(argv[1], MEMFAULT_WIFI_CONFIG_MAX_SIZE);
  app_kvstore_write(MEMFAULT_WIFI_SSID_KEY, (uint8_t *)argv[1], len);

  len = strnlen(argv[2], MEMFAULT_WIFI_CONFIG_MAX_SIZE);
  app_kvstore_write(MEMFAULT_WIFI_AUTH_TYPE_KEY, (uint8_t *)argv[2], len);

  len = strnlen(argv[3], MEMFAULT_WIFI_CONFIG_MAX_SIZE);
  app_kvstore_write(MEMFAULT_WIFI_PASSWORD_KEY, (uint8_t *)argv[3], len);

  return 0;
}

static int prv_send_char(char c) {
  cyhal_uart_putc(&cy_retarget_io_uart_obj, c);
  return 0;
}

// https://infineon.github.io/TARGET_CY8CKIT-062S2-43012/html/group__group__bsp__pins__btn.html
#define CYBSP_USER_BTN1 (P0_4)
#define CYBSP_USER_BTN2 (P1_4)

static void prv_init_user_buttons(void) {
  cyhal_gpio_init(CYBSP_USER_BTN1, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, 1);
  cyhal_gpio_init(CYBSP_USER_BTN2, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, 1);
}

static void prv_check_user_buttons(void) {
  // debounce- only enable these operations 2 seconds after boot
  if (memfault_platform_get_time_since_boot_ms() < (2*1000)) {
    return;
  }

  if (cyhal_gpio_read(CYBSP_USER_BTN1) == 0) {
    MEMFAULT_LOG_INFO("User button 1 pressed, crashing!");
    vTaskDelay(250);

    // trigger a hard fault
    volatile uint32_t *p = (uint32_t *)0x00000000;
    *p = 0x12345678;
  }
  if (cyhal_gpio_read(CYBSP_USER_BTN2) == 0) {
    MEMFAULT_LOG_INFO("User button 2 pressed, asserting!");
    vTaskDelay(250);

    // trigger an assert
    MEMFAULT_ASSERT(0);
  }
}

void memfault_cli_task(void *arg) {
  cyhal_syspm_lock_deepsleep();
  const sMemfaultShellImpl impl = {
    .send_char = prv_send_char,
  };
  memfault_demo_shell_boot(&impl);

  while (1) {
    prv_check_user_buttons();
    uint32_t num_bytes = cyhal_uart_readable(&cy_retarget_io_uart_obj);
    if (num_bytes < 1) {
      // Sleep for a little bit if there was not data
      vTaskDelay(10);
      continue;
    }

    uint8_t rx_byte;
    // data should be available so we should not need to wait
    const uint32_t uart_input_timeout_ms = 1;
    cy_rslt_t result = cyhal_uart_getc(&cy_retarget_io_uart_obj, &rx_byte, uart_input_timeout_ms);
    if (result != CY_RSLT_SUCCESS) {
      MEMFAULT_LOG_ERROR("Unexpected UART read error: 0x%x", (int)result);
      continue;
    }

    memfault_demo_shell_receive_char(rx_byte);
  }
}

void memfault_cli_task_start(void) {
  prv_init_user_buttons();

  xTaskCreate(memfault_cli_task, "MFLT CLI", MEMFAULT_CLI_TASK_SIZE,
              NULL, MEMFAULT_CLI_TASK_PRIORITY, NULL);
}
