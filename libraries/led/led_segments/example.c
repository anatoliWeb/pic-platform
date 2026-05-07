#include "core/compiler.h"
#include "libraries/led/ws2812/ws2812.h"
#include "libraries/led/rgb_utils/rgb_utils.h"
#include "libraries/led/led_segments/led_segments.h"

#define LED_COUNT 24u

static ws2812_color_t g_pixels[LED_COUNT];

void main(void)
{
    ws2812_t strip;
    led_segment_t seg_left;
    led_segment_t seg_right;
    led_segment_t seg_progress;
    led_segment_config_t cfg;
    uint16_t pos;

    (void)ws2812_init(&strip, &PORTC, &TRISC, 0u, g_pixels, LED_COUNT);

    /* Left status zone: LEDs 0..7 */
    cfg.parent_type = LED_SEGMENT_PARENT_STRIP;
    cfg.strip = &strip;
    cfg.matrix = (ws2812_matrix_t*)0;
    cfg.start_index = 0u;
    cfg.length = 8u;
    cfg.reverse = 0u;
    cfg.region_x = 0u;
    cfg.region_y = 0u;
    cfg.region_width = 0u;
    cfg.region_height = 0u;
    (void)led_segment_init(&seg_left, &cfg);

    /* Right status zone: LEDs 8..15 (reversed logical direction). */
    cfg.start_index = 8u;
    cfg.length = 8u;
    cfg.reverse = 1u;
    (void)led_segment_init(&seg_right, &cfg);

    /* Progress bar zone: LEDs 16..23 */
    cfg.start_index = 16u;
    cfg.length = 8u;
    cfg.reverse = 0u;
    (void)led_segment_init(&seg_progress, &cfg);

    while (1)
    {
        /* Fill static zone colors. */
        led_segment_fill(&seg_left, rgb_color(0u, 32u, 0u));
        led_segment_fill(&seg_right, rgb_color(0u, 0u, 32u));

        /* Mirror left segment into right segment. */
        (void)led_segment_mirror(&seg_right, &seg_left);

        /* Draw segmented progress bar. */
        led_segment_clear(&seg_progress);
        for (pos = 0u; pos < seg_progress.length; pos++)
        {
            if (pos <= (uint16_t)(strip.pixels[0].g & 0x07u))
            {
                (void)led_segment_set_pixel(&seg_progress, pos, rgb_color(64u, 16u, 0u));
            }
        }

        /* Animate source state using existing shared framebuffer. */
        strip.pixels[0].g = (uint8_t)((strip.pixels[0].g + 1u) & 0x07u);

        /* Push buffer to LEDs. */
        led_segment_show(&seg_left);
    }
}
