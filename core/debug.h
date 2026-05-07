#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include "libraries/system/uart_debug/uart_debug.h"

/* Backward-compatible aliases for previous debug macros. */
#define DRV_DEBUG_INIT()       do { } while (0)
#define DRV_DEBUG_LOG(msg)     DBG_PRINT(msg)
#define DRV_DEBUG_LOG_HEX(v)   DBG_PRINT_HEX(v)

#endif /* CORE_DEBUG_H */
