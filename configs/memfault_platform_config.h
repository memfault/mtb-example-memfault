//! @file
//!
//! @brief
//! Platform overrides for the default configuration settings in the
//! memfault-firmware-sdk. Default configuration settings can be found in
//! "memfault/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MEMFAULT_COREDUMP_COLLECT_LOG_REGIONS 1
// Currently unavailable for this example app.
#define MEMFAULT_FREERTOS_COLLECT_THREAD_METRICS 0

#ifdef __cplusplus
}
#endif
