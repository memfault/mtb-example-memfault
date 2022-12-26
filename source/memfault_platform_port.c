//! @file
//!
//! Copyright (c) Memfault, Inc.
//! See License.txt for details
//!
//! Glue layer between the Memfault SDK and the underlying platform

#include <inttypes.h>
#include <stdint.h>

#include "cy_device_headers.h"
#include "cy_syslib.h"
#include "memfault/components.h"
#include "memfault_example_app.h"

#if !defined(MEMFAULT_PROJECT_KEY)
  #warning \
    "#define MEMFAULT_PROJECT_KEY must be specified in source/memfault_example_app.h. Navigate to https://mflt.io/project-key to generate one"
  #define MEMFAULT_PROJECT_KEY ""
#endif

sMfltHttpClientConfig g_mflt_http_client_config = {
  .api_key = MEMFAULT_PROJECT_KEY,
};

static char g_mflt_serial_number[sizeof(uint64_t) * 2 + 1] = "UNKNOWN";

void memfault_platform_init_serial_number(void) {
  uint64_t serial = Cy_SysLib_GetUniqueId();
  snprintf(g_mflt_serial_number, sizeof(g_mflt_serial_number), "%" PRIX32 "%" PRIX32,
           (uint32_t)(serial >> 32), (uint32_t)serial);
}

void memfault_platform_get_device_info(sMemfaultDeviceInfo *info) {
  *info = (sMemfaultDeviceInfo){
    // An ID that uniquely identifies the device in your fleet
    // (i.e serial number, mac addr, chip id, etc)
    // Regular expression defining valid device serials: ^[-a-zA-Z0-9_]+$
    .device_serial = g_mflt_serial_number,
    // A name to represent the firmware running on the MCU.
    // (i.e "ble-fw", "main-fw", or a codename for your project)
    .software_type = "app-fw",
    // The version of the "software_type" currently running.
    // "software_type" + "software_version" must uniquely represent
    // a single binary
    .software_version = "1.0.0-dev",
    // The revision of hardware for the device. This value must remain
    // the same for a unique device.
    // (i.e evt, dvt, pvt, or rev1, rev2, etc)
    // Regular expression defining valid hardware versions: ^[-a-zA-Z0-9_\.\+]+$
    .hardware_version = "dvt1",
  };
}
