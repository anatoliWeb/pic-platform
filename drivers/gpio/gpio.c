#include "drivers/gpio/gpio.h"
#include "core/bit_utils.h"

void gpio_init(void)
{
    /* Keep empty by default; projects may add board-level setup if needed. */
}

void gpio_set_output(volatile uint8_t* tris, uint8_t bit)
{
    /* TRIS bit = 0 configures pin as output on PIC18. */
    CLEAR_BIT((*tris), bit);
}

void gpio_set_input(volatile uint8_t* tris, uint8_t bit)
{
    /* TRIS bit = 1 configures pin as input on PIC18. */
    SET_BIT((*tris), bit);
}

void gpio_write_high(volatile uint8_t* port, uint8_t bit)
{
    /* Set output latch/port bit to high. */
    SET_BIT((*port), bit);
}

void gpio_write_low(volatile uint8_t* port, uint8_t bit)
{
    /* Clear output latch/port bit to low. */
    CLEAR_BIT((*port), bit);
}

void gpio_toggle(volatile uint8_t* port, uint8_t bit)
{
    /* Flip current output state. */
    TOGGLE_BIT((*port), bit);
}

uint8_t gpio_read(volatile uint8_t* port, uint8_t bit)
{
    /* Return normalized logic state: 0 or 1. */
    return (uint8_t)READ_BIT((*port), bit);
}
