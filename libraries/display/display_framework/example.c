#include "core/compiler.h"
#include "libraries/display/display_framework/display_framework.h"
#include "libraries/led/ws2812/ws2812.h"
#include "libraries/led/ws2812_matrix/ws2812_matrix.h"
#include "libraries/display/seven_segment/seven_segment.h"

#define MATRIX_WIDTH  8u
#define MATRIX_HEIGHT 8u
#define MATRIX_COUNT  (MATRIX_WIDTH * MATRIX_HEIGHT)

static ws2812_color_t g_pixels[MATRIX_COUNT];

void main(void)
{
    display_t display;
    display_driver_t backend;

    ws2812_t strip;
    ws2812_matrix_t matrix;
    ws2812_matrix_config_t matrix_cfg;

    /* Example 1: WS2812 matrix backend. */
    (void)ws2812_init(&strip, &PORTC, &TRISC, 0u, g_pixels, MATRIX_COUNT);
    matrix_cfg.strip = &strip;
    matrix_cfg.width = MATRIX_WIDTH;
    matrix_cfg.height = MATRIX_HEIGHT;
    matrix_cfg.layout = WS2812_MATRIX_LAYOUT_ROW_MAJOR;
    matrix_cfg.serpentine = 1u;
    matrix_cfg.origin = WS2812_MATRIX_ORIGIN_TOP_LEFT;
    matrix_cfg.reverse = 0u;
    (void)ws2812_matrix_init(&matrix, &matrix_cfg);

    display_backend_ws2812_matrix(&backend);
    (void)display_init(&display, &backend, &matrix, MATRIX_WIDTH, MATRIX_HEIGHT);

    display_clear(&display);
    display_draw_string(&display, 0u, 0u, "A1");
    display_draw_line(&display, 0u, 7u, 7u, 0u, 200u);
    display_draw_rect(&display, 1u, 1u, 6u, 6u, 64u, 0u);
    display_draw_number(&display, 0u, 0u, 42);
    display_update(&display);

    /* Example 2: LCD backend and Seven Segment backend binding pattern. */
    display_backend_lcd_hd44780(&backend);
    (void)display_init(&display, &backend, (void*)0, 16u, 2u);
    display_draw_string(&display, 0u, 0u, "HELLO");

    while (1)
    {
        /* Non-blocking application loop. */
        display_update(&display);
    }
}
