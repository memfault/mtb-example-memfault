//! @file
//!
//! @brief
//! Code manages WIFI access point connection

#include "cyhal.h"

#include "memfault_example_app.h"
#include "memfault/components.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "cy_wcm.h"
#include "cy_wcm_error.h"

#include "cy_lwip.h"

#if !defined(WIFI_SSID)
#warning "WIFI_SSID should be defined in source/memfault_example_app.h"
#define WIFI_SSID ""
#endif

#if !defined(WIFI_PASSWORD)
#warning "WIFI_PASSWORD should be defined in source/memfault_example_app.h"
#define WIFI_PASSWORD ""
#endif

#define WIFI_SECURITY_TYPE                         CY_WCM_SECURITY_WPA2_MIXED_PSK
#define MAX_WIFI_CONN_RETRIES                      (10u)
#define WIFI_CONN_RETRY_INTERVAL_MSEC              (5000)

static cy_rslt_t prv_wifi_ap_connect(cy_wcm_connect_params_t *wifi_conn_param) {
  cy_wcm_ip_address_t ip_address;
  cy_rslt_t result = cy_wcm_connect_ap(wifi_conn_param, &ip_address);

  if (result != CY_RSLT_SUCCESS) {
    return result;
  }

  MEMFAULT_LOG_INFO("Successfully connected to Wi-Fi network '%s'",
                    wifi_conn_param->ap_credentials.SSID);

#if(USE_IPV6_ADDRESS)
  result = cy_wcm_get_ipv6_addr(CY_WCM_INTERFACE_TYPE_STA,
                                CY_WCM_IPV6_LINK_LOCAL, &ip_address,1);
  if (result == CY_RSLT_SUCCESS) {
    MEMFAULT_LOG_INFO("IPv6 address (link-local) assigned: %s",
                      ip6addr_ntoa((const ip6_addr_t*)&ip_address.ip.v6));
  }
#else
  MEMFAULT_LOG_INFO("IPv4 address assigned: %s\n",
                    ip4addr_ntoa((const ip4_addr_t*)&ip_address.ip.v4));
#endif /* USE_IPV6_ADDRESS */
  return result;
}

cy_rslt_t connect_to_wifi_ap(void) {
  // Set the Wi-Fi SSID, password and security type.
  cy_wcm_connect_params_t wifi_conn_param = { 0 };
  memcpy(wifi_conn_param.ap_credentials.SSID, WIFI_SSID, sizeof(WIFI_SSID));
  memcpy(wifi_conn_param.ap_credentials.password, WIFI_PASSWORD, sizeof(WIFI_PASSWORD));
  wifi_conn_param.ap_credentials.security = WIFI_SECURITY_TYPE;


  cy_rslt_t result;
  for (uint32_t conn_retries = 0; conn_retries < MAX_WIFI_CONN_RETRIES; conn_retries++ ) {
    result = prv_wifi_ap_connect(&wifi_conn_param);
    if (result == CY_RSLT_SUCCESS) {
      return result;
    }
    MEMFAULT_LOG_WARN("Connection to Wi-Fi network failed with error code. rv=0x%x."
                      "Retrying in %d ms...", (int)result, WIFI_CONN_RETRY_INTERVAL_MSEC);
    vTaskDelay(pdMS_TO_TICKS(WIFI_CONN_RETRY_INTERVAL_MSEC));
  }

  MEMFAULT_LOG_ERROR("Exceeded maximum Wi-Fi connection attempts\n");
  return result;
}
