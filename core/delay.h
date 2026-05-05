#ifndef CORE_DELAY_H
#define CORE_DELAY_H

#include "core/compiler.h"

/*
 * Unified delay wrappers.
 * Prefer calling these macros from drivers instead of compiler-specific APIs.
 */
#ifndef DRV_DELAY_MS
    #define DRV_DELAY_MS(ms) do { DRV_UNUSED(ms); } while (0)
#endif

#ifndef DRV_DELAY_US
    #define DRV_DELAY_US(us) do { DRV_UNUSED(us); } while (0)
#endif

#endif /* CORE_DELAY_H */
