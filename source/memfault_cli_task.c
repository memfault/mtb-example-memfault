//! @file
//!
//! @brief
//! This file contains task and functions related to handling IO and executing Memfault demo CLI
//! commands (https://mflt.io/demo-cli). These commands can be helpful for quickly experimenting
//! with and testing Memfault functionality

#include "cyhal.h"
#include "memfault/components.h"

#include "cy_retarget_io.h"

#include <FreeRTOS.h>
#include <task.h>

#define MEMFAULT_CLI_TASK_SIZE (1024)
#define MEMFAULT_CLI_TASK_PRIORITY (1)

static int prv_send_char(char c) {
  cyhal_uart_putc(&cy_retarget_io_uart_obj, c);
  return 0;
}

void memfault_cli_task(void *arg) {
  cyhal_syspm_lock_deepsleep();
  const sMemfaultShellImpl impl = {
    .send_char = prv_send_char,
  };
  memfault_demo_shell_boot(&impl);

  while (1) {
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
  xTaskCreate(memfault_cli_task, "MFLT CLI", MEMFAULT_CLI_TASK_SIZE,
              NULL, MEMFAULT_CLI_TASK_PRIORITY, NULL);
}
