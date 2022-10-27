#pragma once

//! @file
//!
//! @brief
//! Helpers for setting up the Memfault example app environment

#include "cyhal.h"

// #define WIFI_SSID "FILLMEIN"
// #define WIFI_PASSWORD "FILLMEIN"

// Get a project key from: https://mflt.io/project-key
// #define MEMFAULT_PROJECT_KEY "YOUR_PROJECT_KEY"

#if defined(__has_include) && __has_include("memfault_example_app_config.h")
#include "memfault_example_app_config.h"
#endif

//! Creates a task which will manage the Memfault CLI
void memfault_cli_task_start(void);

//! Task responsible for handling Memfault CLI commands
void memfault_cli_task(void *arg);

//! Creates a task which will periodically post data to memfault
void memfault_http_task_start(void);

//! Task responsible for posting data to Memfault
void memfault_http_task(void *arg);

//! Attempts to connect to a Wifi AP
//!
//! @return CY_RSLT_SUCCESS if connection succeeded, else error code
cy_rslt_t connect_to_wifi_ap(void);

//! Attempts to connect to a Wifi AP
//!
//! @return CY_RSLT_SUCCESS if connection succeeded, else error code
cy_rslt_t boot_wifi_subsystem(void);
