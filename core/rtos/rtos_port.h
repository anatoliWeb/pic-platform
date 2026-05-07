#ifndef CORE_RTOS_RTOS_PORT_H
#define CORE_RTOS_RTOS_PORT_H

/*
 * Port selection for RTOS abstraction.
 *
 * DRV_USE_FREERTOS = 0 -> bare-metal wrappers.
 * DRV_USE_FREERTOS = 1 -> map wrappers to FreeRTOS APIs.
 */

#ifndef DRV_USE_FREERTOS
#define DRV_USE_FREERTOS 0
#endif

#endif /* CORE_RTOS_RTOS_PORT_H */