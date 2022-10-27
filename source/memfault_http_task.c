/******************************************************************************
* File Name:   memfault_http_task.c
*
* Description: This file contains task and functions related to periodically
* posting diagnostic data collected by the memfault-firmware-sdk to the Memfault cloud
* using HTTPS
*
*******************************************************************************
* Copyright 2019-2021, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "memfault_example_app.h"

/* Header file includes. */
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* FreeRTOS header file. */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/* Standard C header file. */
#include <string.h>

/* Cypress secure socket header file. */
#include "cy_secure_sockets.h"
#include "cy_tls.h"

/* Wi-Fi connection manager header files. */
#include "cy_wcm.h"
#include "cy_wcm_error.h"

/* IP address related header files (part of the lwIP TCP/IP stack). */
#include "ip_addr.h"

/* to use the portable formatting macros */
#include <inttypes.h>

#include "memfault/components.h"
#include "memfault_psoc6_port.h"

#if !defined(MEMFAULT_POST_SEND_INTERVAL_MS)
#define MEMFAULT_POST_SEND_INTERVAL_MS              (60 * 1000)
#endif

#define MEMFAULT_HTTP_TASK_SIZE (5 * 1024)
#define MEMFAULT_HTTP_TASK_PRIORITY (1)

void memfault_http_task(void *arg) {
  cy_wcm_config_t wifi_config = {
    .interface = CY_WCM_INTERFACE_TYPE_STA
  };

  cy_rslt_t result = cy_wcm_init(&wifi_config);
  if (result != CY_RSLT_SUCCESS) {
    MEMFAULT_LOG_ERROR("Wi-Fi Connection Manager initialization failed! rv=0x%x", (int)result);
    CY_ASSERT(0);
  }
  MEMFAULT_LOG_INFO("Wi-Fi Connection Manager initialized.");

  // Note: Must be called after cy_wcm_init()
  memfault_wcm_metrics_boot();

  if (connect_to_wifi_ap() != CY_RSLT_SUCCESS) {
    MEMFAULT_LOG_ERROR("Failed to connect to Wi-Fi AP");
    CY_ASSERT(0);
  }

  //! initialize secure socket library
  result = cy_socket_init();
  if (result != CY_RSLT_SUCCESS) {
    MEMFAULT_LOG_ERROR("Secure Socket initialization failed!");
    CY_ASSERT(0);
  }
  MEMFAULT_LOG_INFO("Secure Socket initialized");

  //! Load root certificates necessary for talking to Memfault servers
  result = cy_tls_load_global_root_ca_certificates(MEMFAULT_ROOT_CERTS_PEM, sizeof(MEMFAULT_ROOT_CERTS_PEM) - 1);
  if (result != CY_RSLT_SUCCESS) {
    MEMFAULT_LOG_ERROR("cy_tls_load_global_root_ca_certificates failed! rv=0x%x", (int)result);
  } else {
    MEMFAULT_LOG_INFO("Global trusted RootCA certificate loaded");
  }

  // post any data available at boot up
  memfault_http_client_post_chunk();

  while (1) {
    // Periodically attempt to post data
    memfault_http_client_post_chunk();
    vTaskDelay(pdMS_TO_TICKS(MEMFAULT_POST_SEND_INTERVAL_MS));
  }
}

void memfault_http_task_start(void) {
  xTaskCreate(memfault_http_task, "MFLT CLI", MEMFAULT_HTTP_TASK_SIZE,
              NULL, MEMFAULT_HTTP_TASK_PRIORITY, NULL);
}
