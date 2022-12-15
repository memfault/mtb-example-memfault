#pragma once

//! @file App-level functions to interface with MTB kv-store

#include <stdbool.h>
#include <stdint.h>

#include "cy_result.h"

#define MEMFAULT_WIFI_SSID_KEY "wifi_ssid"
#define MEMFAULT_WIFI_AUTH_TYPE_KEY "wifi_auth_type"
#define MEMFAULT_WIFI_PASSWORD_KEY "wifi_password"
#define MEMFAULT_WIFI_CONFIG_MAX_SIZE 64

//! Initializes key-value store using MTB kv-store
//!
//! Key value storage is initialized in Flash. Must be called
//! before reading/writing any data to store.
void app_kvstore_init(void);

//! Writes a value to provided key in the store
//!
//! @param key String representing key to write value to
//! @param data Data to store at the provided key
//! @param data_len Length of data to write to the value
//! @returns CY_RSLT_SUCCESS if read succeeded, otherwise error number
cy_rslt_t app_kvstore_write(const char *key, const uint8_t *data, uint32_t data_len);

//! Reads a value from the provided key in the store
//!
//! @param key String representing key to read value from
//! @param data Data to write into from the provided key
//! @param data_len Length of data buffer to read into/Length of data read from store
//! @returns CY_RSLT_SUCCESS if read succeeded, otherwise error number
cy_rslt_t app_kvstore_read(const char *key, uint8_t *data, uint32_t *data_len);

//! Returns true is provided key exists in the store
//!
//! @param key Key to check for existence in the store
//! @returns True if key exists, otherwise false
bool app_kvstore_key_exists(const char *key);
