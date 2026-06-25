/*
 * File: drivers/interrupts/ext_interrupt/ext_interrupt.h
 */

#ifndef DRIVERS_EXT_INTERRUPT_EXT_INTERRUPT_H
#define DRIVERS_EXT_INTERRUPT_EXT_INTERRUPT_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    EXT_INT0 = 0,
    EXT_INT1,
    EXT_INT2
} ext_interrupt_channel_t;

typedef enum
{
    EXT_INT_EDGE_FALLING = 0,
    EXT_INT_EDGE_RISING
} ext_interrupt_edge_t;

typedef void (*ext_interrupt_callback_t)(void);

void ext_interrupt_init(void);

void ext_interrupt_enable(ext_interrupt_channel_t channel);
void ext_interrupt_disable(ext_interrupt_channel_t channel);

void ext_interrupt_set_edge(ext_interrupt_channel_t channel, ext_interrupt_edge_t edge);

void ext_interrupt_set_callback(ext_interrupt_channel_t channel, ext_interrupt_callback_t callback);

void ext_interrupt_clear_flag(ext_interrupt_channel_t channel);
uint8_t ext_interrupt_get_flag(ext_interrupt_channel_t channel);

void ext_interrupt_irq_handler(void);

#endif /* DRIVERS_EXT_INTERRUPT_EXT_INTERRUPT_H */
