#include "core/compiler.h"
#include "core/delay.h"
#include "libraries/display/seven_segment/seven_segment.h"

static const seven_segment_config_t g_cfg =
{
    { &PORTB, &TRISB, 0u }, /* A */
    { &PORTB, &TRISB, 1u }, /* B */
    { &PORTB, &TRISB, 2u }, /* C */
    { &PORTB, &TRISB, 3u }, /* D */
    { &PORTB, &TRISB, 4u }, /* E */
    { &PORTB, &TRISB, 5u }, /* F */
    { &PORTB, &TRISB, 6u }, /* G */
    { &PORTB, &TRISB, 7u }, /* DOT */
    1u,
    SEVEN_SEGMENT_COMMON_CATHODE
};

void main(void)
{
    seven_segment_t display;
    uint8_t i;

    (void)seven_segment_init(&display, &g_cfg);

    while (1)
    {
        /* Show decimal counter 0..9. */
        for (i = 0u; i <= 9u; i++)
        {
            seven_segment_set_digit(&display, i);
            DRV_DELAY_MS(250u);
        }

        /* Show hexadecimal sequence A..F. */
        for (i = 10u; i <= 15u; i++)
        {
            seven_segment_set_hex(&display, i);
            DRV_DELAY_MS(250u);
        }

        /* Blink decimal point. */
        seven_segment_set_dot(&display, 1u);
        DRV_DELAY_MS(200u);
        seven_segment_set_dot(&display, 0u);
        DRV_DELAY_MS(200u);

        seven_segment_clear(&display);
        DRV_DELAY_MS(200u);
    }
}