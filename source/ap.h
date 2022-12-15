#pragma once
//! @file Functions to control the WiFi AP connection

#include <stdint.h>

#include "cy_result.h"

//! Attempts to connect to a Wifi AP
//!
//! @param retries Number of retries to attempt when connecting to an AP
//! @return CY_RSLT_SUCCESS if connection succeeded, else error code
cy_rslt_t connect_to_wifi_ap(const char *ssid, const char *auth_type, const char *password,
                             uint32_t retries);

//! Scans for available WiFi APs
//!
//! Prints information on scanned APs including the SSID and auth type
//! @return CY_RSLT_SUCCESS if scan succeeded, else error code
cy_rslt_t scan_wifi_ap(void);