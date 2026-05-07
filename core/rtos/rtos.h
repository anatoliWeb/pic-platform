#ifndef CORE_RTOS_RTOS_H
#define CORE_RTOS_RTOS_H

#include "core/compiler.h"
#include "core/types.h"
#include "core/rtos/rtos_port.h"

/*
 * Unified RTOS abstraction API.
 *
 * Libraries should use only these APIs and must not call RTOS-native
 * functions directly. The current implementation is bare-metal compatible.
 */

void rtos_delay_ms(uint32_t ms);

/* Future synchronization API placeholders. */
uint8_t rtos_mutex_lock(void* mutex, uint32_t timeout_ms);
void rtos_mutex_unlock(void* mutex);

/* Future queue API placeholders. */
uint8_t rtos_queue_send(void* queue, const void* item, uint32_t timeout_ms);
uint8_t rtos_queue_receive(void* queue, void* item, uint32_t timeout_ms);

#endif /* CORE_RTOS_RTOS_H */