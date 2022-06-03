#pragma once

//! @file

#include "cyhal.h"

// #define WIFI_SSID "FILLMEIN"
// #define WIFI_PASSWORD "FILLMEIN"

// Get a project key from: https://app.memfault.com/organizations/-/projects/-/settings
// #define MEMFAULT_PROJECT_KEY "YOUR_PROJECT_KEY"

#if defined(__has_include) && __has_include("memfault_example_app_config.h")
#include "memfault_example_app_config.h"
#endif

void memfault_cli_task_start(void);
void memfault_cli_task(void *arg);

void memfault_http_task_start(void);
void memfault_http_task(void *arg);

cy_rslt_t connect_to_wifi_ap(void);
