/*
 * File: libraries/system/debug/debug_pins_gpio.c
 *
 * Direct GPIO transport for the PINS backend. Channels are a contiguous
 * range of bits on DRV_DEBUG_PINS_PORT starting at DRV_DEBUG_PINS_START_BIT.
 * The port and tris are driven through the shared GPIO driver.
 */

#include "libraries/system/debug/debug_pins_gpio.h"
#include "drivers/gpio/gpio.h"
#include "core/delay.h"

#if DRV_DEBUG_BACKEND_PINS && DRV_DEBUG_PINS_INTERFACE_GPIO

#if defined(DRV_COMPILER_XC8)
    #include <xc.h>
#elif defined(DRV_COMPILER_C18)
    #include <p18f452.h>
#endif

static uint8_t channel_bit(uint8_t channel)
{
    return (uint8_t)(DRV_DEBUG_PINS_START_BIT + channel);
}

void debug_gpio_pins_init(void)
{
    uint8_t ch;

    for (ch = 0u; ch < DRV_DEBUG_PINS_CHANNEL_COUNT; ch++)
    {
        gpio_set_output(&(DRV_DEBUG_PINS_TRIS), channel_bit(ch));
        gpio_write_low(&(DRV_DEBUG_PINS_PORT), channel_bit(ch));
    }
}

void debug_gpio_pins_set(uint8_t channel, uint8_t state)
{
    if (channel >= DRV_DEBUG_PINS_CHANNEL_COUNT)
    {
        return;
    }

    if (state != 0u)
    {
        gpio_write_high(&(DRV_DEBUG_PINS_PORT), channel_bit(channel));
    }
    else
    {
        gpio_write_low(&(DRV_DEBUG_PINS_PORT), channel_bit(channel));
    }
}

void debug_gpio_pins_pulse(uint8_t channel)
{
    debug_gpio_pins_set(channel, 1u);
    delay_us((uint16_t)DRV_DEBUG_PINS_PULSE_US);
    debug_gpio_pins_set(channel, 0u);
}

void debug_gpio_pins_code(uint8_t value)
{
    uint8_t ch;

    for (ch = 0u; ch < DRV_DEBUG_PINS_CHANNEL_COUNT; ch++)
    {
        debug_gpio_pins_set(ch, (uint8_t)((value >> ch) & 0x01u));
    }
}

void debug_gpio_pins_error(uint8_t code)
{
    uint8_t count;
    uint8_t repeats = (uint8_t)((code & 0x0Fu) + 1u);

    for (count = 0u; count < repeats; count++)
    {
        debug_gpio_pins_pulse(0u);
        delay_ms(50u);
    }
    delay_ms(200u);
}

#endif /* DRV_DEBUG_BACKEND_PINS && GPIO */
