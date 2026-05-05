#ifndef DRIVERS_GPIO_GPIO_H
#define DRIVERS_GPIO_GPIO_H

#include "core/compiler.h"
#include "core/types.h"

/* Optional global GPIO initialization hook. */
void gpio_init(void);

/* Configure selected TRIS bit as output (0). */
void gpio_set_output(volatile uint8_t* tris, uint8_t bit);
/* Configure selected TRIS bit as input (1). */
void gpio_set_input(volatile uint8_t* tris, uint8_t bit);

/* Drive selected output bit to logic high. */
void gpio_write_high(volatile uint8_t* port, uint8_t bit);
/* Drive selected output bit to logic low. */
void gpio_write_low(volatile uint8_t* port, uint8_t bit);
/* Toggle selected output bit. */
void gpio_toggle(volatile uint8_t* port, uint8_t bit);

/* Read selected input/output bit state (0 or 1). */
uint8_t gpio_read(volatile uint8_t* port, uint8_t bit);

#endif /* DRIVERS_GPIO_GPIO_H */
