#ifndef LIBRARIES_LED_LED_SEGMENTS_H
#define LIBRARIES_LED_LED_SEGMENTS_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/led/ws2812/ws2812.h"
#include "libraries/led/rgb_utils/rgb_utils.h"
#include "libraries/led/ws2812_matrix/ws2812_matrix.h"

typedef enum
{
    LED_SEGMENT_PARENT_STRIP = 0,
    LED_SEGMENT_PARENT_MATRIX_REGION
} led_segment_parent_t;

typedef struct
{
    led_segment_parent_t parent_type;

    ws2812_t* strip;
    ws2812_matrix_t* matrix;

    uint16_t start_index;
    uint16_t length;
    uint8_t reverse;

    uint16_t region_x;
    uint16_t region_y;
    uint16_t region_width;
    uint16_t region_height;
} led_segment_config_t;

typedef struct
{
    led_segment_parent_t parent_type;

    ws2812_t* strip;
    ws2812_matrix_t* matrix;

    uint16_t start_index;
    uint16_t length;
    uint8_t reverse;

    uint16_t region_x;
    uint16_t region_y;
    uint16_t region_width;
    uint16_t region_height;

    uint8_t initialized;
} led_segment_t;

uint8_t led_segment_init(led_segment_t* segment, const led_segment_config_t* config);

uint8_t led_segment_set_pixel(led_segment_t* segment, uint16_t index, rgb_color_t color);
void led_segment_fill(led_segment_t* segment, rgb_color_t color);
void led_segment_clear(led_segment_t* segment);
void led_segment_show(led_segment_t* segment);

void led_segment_reverse(led_segment_t* segment, uint8_t enabled);
uint8_t led_segment_mirror(led_segment_t* dst, led_segment_t* src);

#endif /* LIBRARIES_LED_LED_SEGMENTS_H */
