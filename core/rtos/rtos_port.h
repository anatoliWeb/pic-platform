#ifndef CORE_RTOS_RTOS_PORT_H
#define CORE_RTOS_RTOS_PORT_H

/*
 * RTOS mode switch.
 *
 * DRV_USE_FREERTOS = 0: bare-metal mode (default)
 * DRV_USE_FREERTOS = 1: reserved for future FreeRTOS backend
 */
#ifndef DRV_USE_FREERTOS
#define DRV_USE_FREERTOS 0
#endif

#endif /* CORE_RTOS_RTOS_PORT_H */