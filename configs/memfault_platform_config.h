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
#include "ports/cypress/psoc6/templates/memfault_platform_config.h"

#ifdef __cplusplus
}
#endif
