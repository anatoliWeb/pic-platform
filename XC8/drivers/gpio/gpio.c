#include "drivers/gpio/gpio.h"
#include "core/bit_utils.h"

void gpio_init(void)
{
    /* Optional project-level GPIO setup hook. */
}

void gpio_set_output(volatile uint8_t* tris, uint8_t bit)
{
    /* TRIS bit = 0 configures output direction. */
    CLEAR_BIT((*tris), bit);
}

void gpio_set_input(volatile uint8_t* tris, uint8_t bit)
{
    /* TRIS bit = 1 configures input direction. */
    SET_BIT((*tris), bit);
}

void gpio_write_high(volatile uint8_t* port, uint8_t bit)
{
    /* Drive output to logic high. */
    SET_BIT((*port), bit);
}

void gpio_write_low(volatile uint8_t* port, uint8_t bit)
{
    /* Drive output to logic low. */
    CLEAR_BIT((*port), bit);
}

void gpio_toggle(volatile uint8_t* port, uint8_t bit)
{
    /* Toggle output state. */
    TOGGLE_BIT((*port), bit);
}

uint8_t gpio_read(volatile uint8_t* port, uint8_t bit)
{
    /* Return normalized state: 0 or 1. */
    return (uint8_t)READ_BIT((*port), bit);
}
