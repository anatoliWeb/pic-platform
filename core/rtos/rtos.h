#ifndef CORE_RTOS_RTOS_H
#define CORE_RTOS_RTOS_H

#include "core/compiler.h"
#include "core/types.h"
#include "core/rtos/rtos_port.h"

/*
 * Lightweight RTOS abstraction for PIC18.
 *
 * Libraries must call only this API and must not use native RTOS calls
 * directly. Bare-metal mode is always available.
 */

void rtos_init(void);
void rtos_delay_ms(uint32_t ms);
uint32_t rtos_get_tick_ms(void);
uint8_t rtos_is_enabled(void);

/* Optional placeholders for future backend extensions. */
void rtos_yield(void);
void rtos_enter_critical(void);
void rtos_exit_critical(void);

/* Backward-compatible placeholders (future extension points). */
uint8_t rtos_mutex_lock(void* mutex, uint32_t timeout_ms);
void rtos_mutex_unlock(void* mutex);
uint8_t rtos_queue_send(void* queue, const void* item, uint32_t timeout_ms);
uint8_t rtos_queue_receive(void* queue, void* item, uint32_t timeout_ms);

#endif /* CORE_RTOS_RTOS_H */
