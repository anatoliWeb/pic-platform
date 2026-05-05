#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include "core/compiler.h"
#include "core/config.h"
#include "drivers/uart/uart.h"

/*
 * Debug macro template.
 * Enable by defining DRV_DEBUG_ENABLE and DRV_USE_UART.
 */
#if defined(DRV_DEBUG_ENABLE) && (DRV_DEBUG_ENABLE == 1) && defined(DRV_USE_UART) && (DRV_USE_UART == 1)

    #define DRV_DEBUG_INIT()           do { } while (0)
    #define DRV_DEBUG_LOG(msg)         do { uart_write_string(msg); } while (0)
    #define DRV_DEBUG_LOG_HEX(v)       do { uart_write_byte((uint8_t)(v)); } while (0)
    #define DBG_PRINT(str)             uart_write_string(str)

#else

    #define DRV_DEBUG_INIT()           do { } while (0)
    #define DRV_DEBUG_LOG(msg)         do { DRV_UNUSED(msg); } while (0)
    #define DRV_DEBUG_LOG_HEX(v)       do { DRV_UNUSED(v); } while (0)
    #define DBG_PRINT(str)             do { DRV_UNUSED(str); } while (0)

#endif

#endif /* CORE_DEBUG_H */
