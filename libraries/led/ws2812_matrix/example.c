/*
 * File: libraries/led/ws2812_matrix/example.c
 */

#include "core/compiler.h"
#include "libraries/led/ws2812/ws2812.h"
#include "libraries/led/rgb_utils/rgb_utils.h"
#include "libraries/led/ws2812_matrix/ws2812_matrix.h"

#define MATRIX_WIDTH  8u
#define MATRIX_HEIGHT 8u
#define MATRIX_COUNT  (MATRIX_WIDTH * MATRIX_HEIGHT)

static ws2812_color_t g_pixels[MATRIX_COUNT];

void main(void)
{
    ws2812_t strip;
    ws2812_matrix_t matrix;
    ws2812_matrix_config_t cfg;
    uint16_t x;
    uint16_t y;
    uint16_t step;

    /* Initialize strip and matrix context. */
    (void)ws2812_init(&strip, &PORTC, &TRISC, 0u, g_pixels, MATRIX_COUNT);

    cfg.strip = &strip;
    cfg.width = MATRIX_WIDTH;
    cfg.height = MATRIX_HEIGHT;
    cfg.layout = WS2812_MATRIX_LAYOUT_ROW_MAJOR;
    cfg.serpentine = 1u;
    cfg.origin = WS2812_MATRIX_ORIGIN_TOP_LEFT;
    cfg.reverse = 0u;
    (void)ws2812_matrix_init(&matrix, &cfg);

    step = 0u;
    while (1)
    {
        /* Moving pixel demo. */
        ws2812_matrix_clear(&matrix);
        x = (uint16_t)(step % MATRIX_WIDTH);
        y = (uint16_t)((step / MATRIX_WIDTH) % MATRIX_HEIGHT);
        (void)ws2812_matrix_set_pixel(&matrix, x, y, rgb_color(255u, 0u, 0u));
        ws2812_matrix_show(&matrix);

        /* Diagonal line overlay demo. */
        ws2812_matrix_draw_line(&matrix, 0u, 0u, 7u, 7u, rgb_color(0u, 64u, 255u));
        ws2812_matrix_show(&matrix);

        /* Fill pattern demo. */
        ws2812_matrix_fill(&matrix, rgb_color(0u, 16u, 0u));
        (void)ws2812_matrix_set_pixel(&matrix, x, y, rgb_color(255u, 255u, 0u));
        ws2812_matrix_show(&matrix);

        step++;
    }
}
