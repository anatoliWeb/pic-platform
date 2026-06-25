/*
 * File: libraries/display/seven_segment_mux/example.c
 */

#include "core/compiler.h"
#include "libraries/display/seven_segment_mux/seven_segment_mux.h"

static const seven_segment_pin_t g_digit_pins[4] =
{
    { &PORTD, &TRISD, 0u },
    { &PORTD, &TRISD, 1u },
    { &PORTD, &TRISD, 2u },
    { &PORTD, &TRISD, 3u }
};

static const seven_segment_mux_config_t g_cfg =
{
    {
        { &PORTB, &TRISB, 0u },
        { &PORTB, &TRISB, 1u },
        { &PORTB, &TRISB, 2u },
        { &PORTB, &TRISB, 3u },
        { &PORTB, &TRISB, 4u },
        { &PORTB, &TRISB, 5u },
        { &PORTB, &TRISB, 6u },
        { &PORTB, &TRISB, 7u },
        1u,
        SEVEN_SEGMENT_COMMON_CATHODE
    },
    g_digit_pins,
    4u,
    1u,
    1u
};

void main(void)
{
    seven_segment_mux_t display;
    uint16_t counter = 0u;
    uint16_t div = 0u;

    (void)seven_segment_mux_init(&display, &g_cfg);

    while (1)
    {
        /* Keep refresh call very frequent and non-blocking. */
        seven_segment_mux_update(&display);

        div++;
        if (div >= 500u)
        {
            div = 0u;
            seven_segment_mux_set_number(&display, (int32_t)counter);

            /* Optional blinking dot on last digit. */
            seven_segment_mux_set_dot(&display, 3u, (uint8_t)((counter & 0x01u) != 0u));

            counter++;
            if (counter > 9999u)
            {
                counter = 0u;
            }
        }
    }
}