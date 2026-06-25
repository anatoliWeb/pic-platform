/*
 * File: drivers/gpio/gpio.h
 */

#ifndef DRIVERS_GPIO_GPIO_H
#define DRIVERS_GPIO_GPIO_H

#include "core/compiler.h"
#include "core/types.h"

void gpio_init(void);

void gpio_set_output(volatile uint8_t* tris, uint8_t bit);
void gpio_set_input(volatile uint8_t* tris, uint8_t bit);

void gpio_write_high(volatile uint8_t* port, uint8_t bit);
void gpio_write_low(volatile uint8_t* port, uint8_t bit);
void gpio_toggle(volatile uint8_t* port, uint8_t bit);

uint8_t gpio_read(volatile uint8_t* port, uint8_t bit);

void gpio_reg_set_output(volatile uint8_t* tris, uint8_t bit);
void gpio_reg_set_input(volatile uint8_t* tris, uint8_t bit);
void gpio_reg_write_high(volatile uint8_t* port, uint8_t bit);
void gpio_reg_write_low(volatile uint8_t* port, uint8_t bit);
void gpio_reg_toggle(volatile uint8_t* port, uint8_t bit);
uint8_t gpio_reg_read(volatile uint8_t* port, uint8_t bit);

#endif /* DRIVERS_GPIO_GPIO_H */
