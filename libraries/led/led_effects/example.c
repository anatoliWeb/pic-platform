#include "core/compiler.h"
#include "drivers/gpio/gpio.h"
#include "libraries/led/ws2812/ws2812.h"
#include "libraries/led/led_effects/led_effects.h"

#define LED_COUNT 12u
#define BTN_PIN   1u

static ws2812_color_t g_pixels[LED_COUNT];

void main(void)
{
    ws2812_t strip;
    led_effects_t fx;
    led_effects_config_t cfg;
    uint8_t last_btn = 0u;
    uint8_t btn;

    (void)ws2812_init(&strip, &PORTC, &TRISC, 0u, g_pixels, LED_COUNT);

    gpio_set_input(&TRISB, BTN_PIN);

    cfg.strip = &strip;
    cfg.led_count = LED_COUNT;
    cfg.brightness = 96u;
    cfg.speed = 2u;
    cfg.color = rgb_color(255u, 64u, 0u);

    (void)led_effects_init(&fx, &cfg);
    led_effects_set_mode(&fx, LED_EFFECT_MODE_RAINBOW_CYCLE);

    while (1)
    {
        /* Non-blocking incremental animation update. */
        led_effects_update(&fx);

        /* Button edge switches to next mode. */
        btn = gpio_read(&PORTB, BTN_PIN);
        if ((btn != 0u) && (last_btn == 0u))
        {
            switch (fx.mode)
            {
                case LED_EFFECT_MODE_RAINBOW_CYCLE:
                    led_effects_set_mode(&fx, LED_EFFECT_MODE_BREATHING);
                    break;
                case LED_EFFECT_MODE_BREATHING:
                    led_effects_set_mode(&fx, LED_EFFECT_MODE_SCANNER);
                    break;
                case LED_EFFECT_MODE_SCANNER:
                    led_effects_set_mode(&fx, LED_EFFECT_MODE_COLOR_WIPE);
                    break;
                default:
                    led_effects_set_mode(&fx, LED_EFFECT_MODE_RAINBOW_CYCLE);
                    break;
            }
        }
        last_btn = btn;
    }
}