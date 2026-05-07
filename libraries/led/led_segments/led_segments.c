#include "libraries/led/led_segments/led_segments.h"

static ws2812_color_t led_segment_to_ws_color(rgb_color_t color)
{
    ws2812_color_t ws_color;
    ws_color.r = color.r;
    ws_color.g = color.g;
    ws_color.b = color.b;
    return ws_color;
}

static uint16_t led_segment_map_linear_index(const led_segment_t* segment, uint16_t index)
{
    uint16_t logical_index;

    if (segment->length == 0u)
    {
        return 0xFFFFu;
    }

    logical_index = index;
    if (segment->reverse != 0u)
    {
        logical_index = (uint16_t)(segment->length - 1u - index);
    }

    return (uint16_t)(segment->start_index + logical_index);
}

static uint8_t led_segment_map_matrix_index(const led_segment_t* segment,
                                            uint16_t index,
                                            uint16_t* out_x,
                                            uint16_t* out_y)
{
    uint16_t logical_index;
    uint16_t local_x;
    uint16_t local_y;

    if ((segment->region_width == 0u) || (segment->region_height == 0u))
    {
        return 0u;
    }

    logical_index = index;
    if (segment->reverse != 0u)
    {
        logical_index = (uint16_t)(segment->length - 1u - index);
    }

    local_x = (uint16_t)(logical_index % segment->region_width);
    local_y = (uint16_t)(logical_index / segment->region_width);

    *out_x = (uint16_t)(segment->region_x + local_x);
    *out_y = (uint16_t)(segment->region_y + local_y);
    return 1u;
}

uint8_t led_segment_init(led_segment_t* segment, const led_segment_config_t* config)
{
    uint32_t max_linear;
    uint32_t region_size;

    if ((segment == (led_segment_t*)0) || (config == (const led_segment_config_t*)0))
    {
        return 0u;
    }

    if (config->length == 0u)
    {
        return 0u;
    }

    if (config->parent_type == LED_SEGMENT_PARENT_STRIP)
    {
        if (config->strip == (ws2812_t*)0)
        {
            return 0u;
        }

        max_linear = (uint32_t)config->start_index + (uint32_t)config->length;
        if (max_linear > (uint32_t)config->strip->led_count)
        {
            return 0u;
        }
    }
    else
    {
        if (config->matrix == (ws2812_matrix_t*)0)
        {
            return 0u;
        }

        region_size = (uint32_t)config->region_width * (uint32_t)config->region_height;
        if ((region_size == 0u) || (region_size < (uint32_t)config->length))
        {
            return 0u;
        }

        if (((uint32_t)config->region_x + (uint32_t)config->region_width) > (uint32_t)config->matrix->width)
        {
            return 0u;
        }
        if (((uint32_t)config->region_y + (uint32_t)config->region_height) > (uint32_t)config->matrix->height)
        {
            return 0u;
        }
    }

    segment->parent_type = config->parent_type;
    segment->strip = config->strip;
    segment->matrix = config->matrix;
    segment->start_index = config->start_index;
    segment->length = config->length;
    segment->reverse = (uint8_t)(config->reverse != 0u ? 1u : 0u);
    segment->region_x = config->region_x;
    segment->region_y = config->region_y;
    segment->region_width = config->region_width;
    segment->region_height = config->region_height;
    segment->initialized = 1u;

    return 1u;
}

uint8_t led_segment_set_pixel(led_segment_t* segment, uint16_t index, rgb_color_t color)
{
    uint16_t mapped;
    uint16_t x;
    uint16_t y;

    if ((segment == (led_segment_t*)0) || (segment->initialized == 0u))
    {
        return 0u;
    }

    if (index >= segment->length)
    {
        return 0u;
    }

    if (segment->parent_type == LED_SEGMENT_PARENT_STRIP)
    {
        mapped = led_segment_map_linear_index(segment, index);
        ws2812_set_pixel(segment->strip, mapped, led_segment_to_ws_color(color));
        return 1u;
    }

    if (led_segment_map_matrix_index(segment, index, &x, &y) == 0u)
    {
        return 0u;
    }

    return ws2812_matrix_set_pixel(segment->matrix, x, y, color);
}

void led_segment_fill(led_segment_t* segment, rgb_color_t color)
{
    uint16_t i;

    if ((segment == (led_segment_t*)0) || (segment->initialized == 0u))
    {
        return;
    }

    for (i = 0u; i < segment->length; i++)
    {
        (void)led_segment_set_pixel(segment, i, color);
    }
}

void led_segment_clear(led_segment_t* segment)
{
    led_segment_fill(segment, rgb_color(0u, 0u, 0u));
}

void led_segment_show(led_segment_t* segment)
{
    if ((segment == (led_segment_t*)0) || (segment->initialized == 0u))
    {
        return;
    }

    if (segment->parent_type == LED_SEGMENT_PARENT_STRIP)
    {
        ws2812_show(segment->strip);
    }
    else
    {
        ws2812_matrix_show(segment->matrix);
    }
}

void led_segment_reverse(led_segment_t* segment, uint8_t enabled)
{
    if ((segment == (led_segment_t*)0) || (segment->initialized == 0u))
    {
        return;
    }

    segment->reverse = (uint8_t)(enabled != 0u ? 1u : 0u);
}

uint8_t led_segment_mirror(led_segment_t* dst, led_segment_t* src)
{
    uint16_t i;
    uint16_t src_index;
    ws2812_color_t color;
    uint16_t strip_index;
    uint16_t x;
    uint16_t y;
    uint16_t matrix_linear;

    if ((dst == (led_segment_t*)0) || (src == (led_segment_t*)0))
    {
        return 0u;
    }

    if ((dst->initialized == 0u) || (src->initialized == 0u))
    {
        return 0u;
    }

    if (dst->length != src->length)
    {
        return 0u;
    }

    for (i = 0u; i < src->length; i++)
    {
        src_index = (uint16_t)(src->length - 1u - i);

        if (src->parent_type == LED_SEGMENT_PARENT_STRIP)
        {
            strip_index = led_segment_map_linear_index(src, src_index);
            color = src->strip->pixels[strip_index];
        }
        else
        {
            if (led_segment_map_matrix_index(src, src_index, &x, &y) == 0u)
            {
                return 0u;
            }

            matrix_linear = ws2812_matrix_get_index(src->matrix, x, y);
            if (matrix_linear == WS2812_MATRIX_INVALID_INDEX)
            {
                return 0u;
            }
            color = src->matrix->strip->pixels[matrix_linear];
        }

        if (dst->parent_type == LED_SEGMENT_PARENT_STRIP)
        {
            strip_index = led_segment_map_linear_index(dst, i);
            ws2812_set_pixel(dst->strip, strip_index, color);
        }
        else
        {
            if (led_segment_map_matrix_index(dst, i, &x, &y) == 0u)
            {
                return 0u;
            }
            (void)ws2812_matrix_set_pixel(dst->matrix, x, y, rgb_color(color.r, color.g, color.b));
        }
    }

    return 1u;
}
