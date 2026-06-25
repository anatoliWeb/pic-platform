/*
 * File: XC8/drivers/gpio/gpio.c
 */

#include "drivers/gpio/gpio.h"

#include "core/bit_utils.h"

void gpio_init(void)
{
    /* Optional project-level GPIO setup hook. */
}

static void gpio_reg_write_bit(volatile uint8_t* reg,
                               uint8_t bit,
                               uint8_t value)
{
    uint8_t current_value;
    uint8_t mask;

    if ((reg == (volatile uint8_t*)0) || (bit >= 8u))
    {
        return;
    }

    current_value = *reg;
    mask = (uint8_t)((uint8_t)1u << bit);

    if (value != 0u)
    {
        current_value = (uint8_t)(current_value | mask);
    }
    else
    {
        current_value = (uint8_t)(current_value & (uint8_t)~mask);
    }

    *reg = current_value;
}

void gpio_reg_set_output(volatile uint8_t* tris, uint8_t bit)
{
    /* TRIS bit = 0 configures output direction. */
    gpio_reg_write_bit(tris, bit, 0u);
}

void gpio_reg_set_input(volatile uint8_t* tris, uint8_t bit)
{
    /* TRIS bit = 1 configures input direction. */
    gpio_reg_write_bit(tris, bit, 1u);
}

void gpio_reg_write_high(volatile uint8_t* port, uint8_t bit)
{
    /* Drive output to logic high. */
    gpio_reg_write_bit(port, bit, 1u);
}

void gpio_reg_write_low(volatile uint8_t* port, uint8_t bit)
{
    /* Drive output to logic low. */
    gpio_reg_write_bit(port, bit, 0u);
}

void gpio_reg_toggle(volatile uint8_t* port, uint8_t bit)
{
    uint8_t current_value;
    uint8_t mask;

    if ((port == (volatile uint8_t*)0) || (bit >= 8u))
    {
        return;
    }

    current_value = *port;
    mask = (uint8_t)((uint8_t)1u << bit);
    current_value = (uint8_t)(current_value ^ mask);
    *port = current_value;
}

uint8_t gpio_reg_read(volatile uint8_t* port, uint8_t bit)
{
    uint8_t current_value;

    /* Return normalized state: 0 or 1. */
    if ((port == (volatile uint8_t*)0) || (bit >= 8u))
    {
        return 0u;
    }

    current_value = *port;
    return (uint8_t)((current_value >> bit) & 0x01u);
}

void gpio_set_output(volatile uint8_t* tris, uint8_t bit)
{
    gpio_reg_set_output(tris, bit);
}

void gpio_set_input(volatile uint8_t* tris, uint8_t bit)
{
    gpio_reg_set_input(tris, bit);
}

void gpio_write_high(volatile uint8_t* port, uint8_t bit)
{
    gpio_reg_write_high(port, bit);
}

void gpio_write_low(volatile uint8_t* port, uint8_t bit)
{
    gpio_reg_write_low(port, bit);
}

void gpio_toggle(volatile uint8_t* port, uint8_t bit)
{
    gpio_reg_toggle(port, bit);
}

uint8_t gpio_read(volatile uint8_t* port, uint8_t bit)
{
    return gpio_reg_read(port, bit);
}
