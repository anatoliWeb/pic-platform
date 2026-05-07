#ifndef CORE_RTOS_RTOS_H
#define CORE_RTOS_RTOS_H

#include "core/compiler.h"
#include "core/types.h"

/*
 * RTOS abstraction layer.
 *
 * Default behavior is bare-metal compatible no-op wrappers.
 * Future FreeRTOS integration should be implemented in rtos_port.h/.c
 * without changing upper-layer library APIs.
 */

void rtos_delay_ms(uint32_t ms);
uint8_t rtos_mutex_lock(void* mutex, uint32_t timeout_ms);
void rtos_mutex_unlock(void* mutex);
uint8_t rtos_queue_send(void* queue, const void* item, uint32_t timeout_ms);
uint8_t rtos_queue_receive(void* queue, void* item, uint32_t timeout_ms);

#endif /* CORE_RTOS_RTOS_H */