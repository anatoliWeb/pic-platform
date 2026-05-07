#ifndef DRIVERS_PORTB_CHANGE_PORTB_CHANGE_H
#define DRIVERS_PORTB_CHANGE_PORTB_CHANGE_H

#include "core/compiler.h"
#include "core/types.h"

typedef void (*portb_change_callback_t)(uint8_t changed_mask, uint8_t current_state);

void portb_change_init(void);
void portb_change_enable(void);
void portb_change_disable(void);

uint8_t portb_change_read_port(void);
uint8_t portb_change_get_last_state(void);
uint8_t portb_change_get_changed_mask(void);

void portb_change_set_callback(portb_change_callback_t cb);

void portb_change_irq_handler(void);

#endif /* DRIVERS_PORTB_CHANGE_PORTB_CHANGE_H */
