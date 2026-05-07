#include "core/compiler.h"
#include "libraries/display/seven_segment_mux/seven_segment_mux.h"
#include "libraries/input/shared_segment_buttons/shared_segment_buttons.h"

static const seven_segment_pin_t g_digit_pins[4] =
{
    { &PORTD, &TRISD, 0u },
    { &PORTD, &TRISD, 1u },
    { &PORTD, &TRISD, 2u },
    { &PORTD, &TRISD, 3u }
};

static const seven_segment_mux_config_t g_display_cfg =
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

/*
 * Example shared mapping:
 * key '+' -> digit0 + segment A
 * key '-' -> digit1 + segment A
 */
static const shared_segment_button_map_t g_button_map[2] =
{
    { 0u, SHARED_SEG_SEG_A, '+' },
    { 1u, SHARED_SEG_SEG_A, '-' }
};

static const shared_segment_buttons_config_t g_buttons_cfg =
{
    (const seven_segment_mux_t*)0,
    g_button_map,
    2u,
    0u,
    2u
};

void main(void)
{
    seven_segment_mux_t display;
    shared_segment_buttons_t buttons;
    shared_segment_buttons_config_t btn_cfg;
    int16_t counter = 0;
    char key;

    (void)seven_segment_mux_init(&display, &g_display_cfg);

    btn_cfg = g_buttons_cfg;
    btn_cfg.display = &display;
    (void)shared_segment_buttons_init(&buttons, &btn_cfg);

    while (1)
    {
        /* Keep display refresh active and non-blocking. */
        seven_segment_mux_update(&display);

        /* Scan one shared button per call (non-blocking). */
        shared_segment_buttons_update(&buttons);

        key = shared_segment_buttons_get_key(&buttons);
        if (key == '+')
        {
            counter++;
        }
        else if (key == '-')
        {
            counter--;
        }

        seven_segment_mux_set_number(&display, counter);
    }
}