/*
 * File: core/rtos/rtos_port.h
 */

#ifndef CORE_RTOS_RTOS_PORT_H
#define CORE_RTOS_RTOS_PORT_H

/*
 * RTOS mode switch:
 * 0 - bare-metal mode (default)
 * 1 - reserved for future FreeRTOS backend
 */
#ifndef DRV_USE_FREERTOS
#define DRV_USE_FREERTOS 0
#endif

#if (DRV_USE_FREERTOS != 0) && (DRV_USE_FREERTOS != 1)
#error "DRV_USE_FREERTOS must be 0 or 1"
#endif

/*
 * Optional cooperative yield hook for bare-metal mode.
 * Applications may define this macro to call scheduler/event pumping code.
 */
#ifndef DRV_RTOS_YIELD_HOOK
#define DRV_RTOS_YIELD_HOOK() do { } while (0)
#endif

#endif /* CORE_RTOS_RTOS_PORT_H */
