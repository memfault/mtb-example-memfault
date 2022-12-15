//! @file
//!
//! @brief
//! Code manages WIFI access point connection

#include "ap.h"

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "app_kvstore.h"
#include "cy_wcm.h"
#include "cy_wcm_error.h"
#include "cyhal.h"
#include "memfault/components.h"

/* IP address related header files (part of the lwIP TCP/IP stack). */
#include "ip_addr.h"

#define WIFI_CONN_RETRY_INTERVAL_MSEC              (5000)

//! Helper function to convert from cy_wcm_security_t value to a string
static const char *wifi_utils_authtype_to_str(cy_wcm_security_t sec) {
  switch (sec) {
    case CY_WCM_SECURITY_OPEN:
      return "open";
    case CY_WCM_SECURITY_WEP_PSK:
    case CY_WCM_SECURITY_WEP_SHARED:
      return "wep";
    case CY_WCM_SECURITY_WPA2_MIXED_PSK:
      return "wpa2";
    case CY_WCM_SECURITY_WPA_AES_PSK:
      return "wpa_aes";
    case CY_WCM_SECURITY_WPA2_AES_PSK:
      return "wpa2_aes";
    case CY_WCM_SECURITY_WPA_TKIP_PSK:
      return "wpa_tkip";
    case CY_WCM_SECURITY_WPA2_TKIP_PSK:
      return "wpa2_tkip";
    case CY_WCM_SECURITY_WPA3_SAE:
      return "wpa3";
    case CY_WCM_SECURITY_WPA3_WPA2_PSK:
      return "wpa3_wpa2";
    case CY_WCM_SECURITY_UNKNOWN:
      return "Unknown";
    default:
      return "Unsupported";
  }
}

//! Helper function to convert a string to cy_wcm_security_t
//!
//! @param auth_str String representing an auth type
//! @returns cy_wcm_security_t representing the auth type
static cy_wcm_security_t wifi_utils_str_to_authtype(const char *auth_str) {
  if (strcmp(auth_str, "open") == 0) {
    return CY_WCM_SECURITY_OPEN;
  } else if (strcmp(auth_str, "wpa2_tkip") == 0) {
    return CY_WCM_SECURITY_WPA2_TKIP_PSK;
  } else if (strcmp(auth_str, "wpa2_aes") == 0) {
    return CY_WCM_SECURITY_WPA2_AES_PSK;
  } else if (strcmp(auth_str, "wpa2") == 0) {
    return CY_WCM_SECURITY_WPA2_MIXED_PSK;
  } else if (strcmp(auth_str, "wpa_aes") == 0) {
    return CY_WCM_SECURITY_WPA_AES_PSK;
  } else if (strcmp(auth_str, "wpa_tkip") == 0) {
    return CY_WCM_SECURITY_WPA_TKIP_PSK;
  } else if (strcmp(auth_str, "wpa3") == 0) {
    return CY_WCM_SECURITY_WPA3_SAE;
  } else if (strcmp(auth_str, "wpa3_wpa2") == 0) {
    return CY_WCM_SECURITY_WPA3_WPA2_PSK;
  } else {
    MEMFAULT_LOG_ERROR("Unsupported auth type: '%s'", auth_str);
    return CY_WCM_SECURITY_UNKNOWN;
  }
}

//! Helper function to build a cy_wcm_connect_params_t struct from SSID, auth type, and password
//!
//! @param ssid String of the network SSID
//! @param auth_type String of the network auth type
//! @param password String of the network password
//! @param connect_params Pointer of struct to build with SSID, auth type, and password
//! @returns 0 on success, otherwise -1 on error
static int convert_to_wcm_connect_params(const char *ssid, const char *auth_type,
                                         const char *password,
                                         cy_wcm_connect_params_t *connect_params) {
  memset(connect_params, 0, sizeof(cy_wcm_connect_params_t));

  // command format: join <ssid> <sec type> [key] [channel/band]

  // ssid
  memcpy(connect_params->ap_credentials.SSID, ssid, strnlen(ssid, MEMFAULT_WIFI_CONFIG_MAX_SIZE));

  // security type
  connect_params->ap_credentials.security = wifi_utils_str_to_authtype(auth_type);
  if (connect_params->ap_credentials.security == CY_WCM_SECURITY_UNKNOWN) {
    MEMFAULT_LOG_ERROR("Cannot connect due to unsupported auth type.");
    return -1;
  }

  // passkey
  if (connect_params->ap_credentials.security != CY_WCM_SECURITY_OPEN) {
    memcpy(connect_params->ap_credentials.password, password,
           strnlen(password, MEMFAULT_WIFI_CONFIG_MAX_SIZE));
  }

  return 0;
}

//! Helper function to connect to AP using provided config
static cy_rslt_t prv_wifi_ap_connect(cy_wcm_connect_params_t *wifi_conn_param) {
  cy_wcm_ip_address_t ip_address;

  if (cy_wcm_is_connected_to_ap()) {
    cy_wcm_disconnect_ap();
  }

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

//! Callback function used to log scan results to console during a scan.
static void prv_scan_result_cb(cy_wcm_scan_result_t *result_ptr, void *user_data,
                               cy_wcm_scan_status_t status) {
  if (status == CY_WCM_SCAN_INCOMPLETE) {
    const char *auth_type = wifi_utils_authtype_to_str(result_ptr->security);

    MEMFAULT_LOG_INFO("%-20s %-14s %-10d %-7d %02X:%02X:%02X:%02X:%02X:%02X", result_ptr->SSID,
                      auth_type, result_ptr->signal_strength, result_ptr->channel,
                      result_ptr->BSSID[0], result_ptr->BSSID[1], result_ptr->BSSID[2],
                      result_ptr->BSSID[3], result_ptr->BSSID[4], result_ptr->BSSID[5]);
  } else if (status == CY_WCM_SCAN_COMPLETE) {
    MEMFAULT_LOG_INFO("#### Scan Results END ####");
  }
}

cy_rslt_t connect_to_wifi_ap(const char *ssid, const char *auth_type, const char *password,
                             uint32_t retries) {
  // Set the Wi-Fi SSID, password and security type.
  if (ssid == NULL || password == NULL) {
    return -1;
  }

  cy_wcm_connect_params_t wifi_conn_param = {0};
  cy_rslt_t result = convert_to_wcm_connect_params(ssid, auth_type, password, &wifi_conn_param);
  if (result != CY_RSLT_SUCCESS) {
    MEMFAULT_LOG_ERROR("Could not parse args correctly");
    return result;
  }

  for (uint32_t conn_retries = 0; conn_retries < retries; conn_retries++) {
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

cy_rslt_t scan_wifi_ap(void) {
  cy_rslt_t res = cy_wcm_start_scan(prv_scan_result_cb, NULL, NULL);
  if (res != CY_RSLT_SUCCESS && res != CY_RSLT_WCM_SCAN_IN_PROGRESS) {
    MEMFAULT_LOG_ERROR("Error while scanning. Res: %u", (unsigned int)res);
  }

  return res;
}
