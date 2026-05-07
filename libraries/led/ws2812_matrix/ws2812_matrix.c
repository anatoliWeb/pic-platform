#include "libraries/led/ws2812_matrix/ws2812_matrix.h"

static ws2812_color_t ws2812_matrix_to_ws_color(rgb_color_t color)
{
    ws2812_color_t ws_color;
    ws_color.r = color.r;
    ws_color.g = color.g;
    ws_color.b = color.b;
    return ws_color;
}

static void ws2812_matrix_apply_origin(const ws2812_matrix_t* matrix,
                                       uint16_t x,
                                       uint16_t y,
                                       uint16_t* out_x,
                                       uint16_t* out_y)
{
    uint16_t mx;
    uint16_t my;

    mx = x;
    my = y;

    if ((matrix->origin == WS2812_MATRIX_ORIGIN_TOP_RIGHT) ||
        (matrix->origin == WS2812_MATRIX_ORIGIN_BOTTOM_RIGHT))
    {
        mx = (uint16_t)(matrix->width - 1u - x);
    }

    if ((matrix->origin == WS2812_MATRIX_ORIGIN_BOTTOM_LEFT) ||
        (matrix->origin == WS2812_MATRIX_ORIGIN_BOTTOM_RIGHT))
    {
        my = (uint16_t)(matrix->height - 1u - y);
    }

    *out_x = mx;
    *out_y = my;
}

uint8_t ws2812_matrix_init(ws2812_matrix_t* matrix, const ws2812_matrix_config_t* config)
{
    uint32_t required_count;

    if ((matrix == (ws2812_matrix_t*)0) || (config == (const ws2812_matrix_config_t*)0))
    {
        return 0u;
    }

    if ((config->strip == (ws2812_t*)0) || (config->width == 0u) || (config->height == 0u))
    {
        return 0u;
    }

    required_count = (uint32_t)config->width * (uint32_t)config->height;
    if (required_count > (uint32_t)config->strip->led_count)
    {
        return 0u;
    }

    matrix->strip = config->strip;
    matrix->width = config->width;
    matrix->height = config->height;
    matrix->layout = config->layout;
    matrix->serpentine = (uint8_t)(config->serpentine != 0u ? 1u : 0u);
    matrix->origin = config->origin;
    matrix->reverse = (uint8_t)(config->reverse != 0u ? 1u : 0u);
    matrix->initialized = 1u;

    return 1u;
}

uint16_t ws2812_matrix_get_index(const ws2812_matrix_t* matrix, uint16_t x, uint16_t y)
{
    uint16_t mx;
    uint16_t my;
    uint16_t row_or_col;
    uint16_t offset;
    uint16_t base_index;
    uint16_t total;

    if ((matrix == (const ws2812_matrix_t*)0) || (matrix->initialized == 0u))
    {
        return WS2812_MATRIX_INVALID_INDEX;
    }

    if ((x >= matrix->width) || (y >= matrix->height))
    {
        return WS2812_MATRIX_INVALID_INDEX;
    }

    ws2812_matrix_apply_origin(matrix, x, y, &mx, &my);

    if (matrix->layout == WS2812_MATRIX_LAYOUT_ROW_MAJOR)
    {
        row_or_col = my;
        base_index = (uint16_t)(my * matrix->width);
        if ((matrix->serpentine != 0u) && ((row_or_col & 0x01u) != 0u))
        {
            offset = (uint16_t)(matrix->width - 1u - mx);
        }
        else
        {
            offset = mx;
        }
    }
    else
    {
        row_or_col = mx;
        base_index = (uint16_t)(mx * matrix->height);
        if ((matrix->serpentine != 0u) && ((row_or_col & 0x01u) != 0u))
        {
            offset = (uint16_t)(matrix->height - 1u - my);
        }
        else
        {
            offset = my;
        }
    }

    total = (uint16_t)(matrix->width * matrix->height);
    if (matrix->reverse != 0u)
    {
        return (uint16_t)((total - 1u) - (base_index + offset));
    }

    return (uint16_t)(base_index + offset);
}

uint8_t ws2812_matrix_set_pixel(ws2812_matrix_t* matrix, uint16_t x, uint16_t y, rgb_color_t color)
{
    uint16_t index;

    if ((matrix == (ws2812_matrix_t*)0) || (matrix->initialized == 0u))
    {
        return 0u;
    }

    index = ws2812_matrix_get_index(matrix, x, y);
    if (index == WS2812_MATRIX_INVALID_INDEX)
    {
        return 0u;
    }

    ws2812_set_pixel(matrix->strip, index, ws2812_matrix_to_ws_color(color));
    return 1u;
}

void ws2812_matrix_clear(ws2812_matrix_t* matrix)
{
    if ((matrix == (ws2812_matrix_t*)0) || (matrix->initialized == 0u))
    {
        return;
    }

    ws2812_clear(matrix->strip);
}

void ws2812_matrix_fill(ws2812_matrix_t* matrix, rgb_color_t color)
{
    if ((matrix == (ws2812_matrix_t*)0) || (matrix->initialized == 0u))
    {
        return;
    }

    ws2812_set_all(matrix->strip, ws2812_matrix_to_ws_color(color));
}

void ws2812_matrix_show(ws2812_matrix_t* matrix)
{
    if ((matrix == (ws2812_matrix_t*)0) || (matrix->initialized == 0u))
    {
        return;
    }

    ws2812_show(matrix->strip);
}

uint8_t ws2812_matrix_draw_line(ws2812_matrix_t* matrix,
                                uint16_t x0,
                                uint16_t y0,
                                uint16_t x1,
                                uint16_t y1,
                                rgb_color_t color)
{
    int16_t dx;
    int16_t dy;
    int16_t sx;
    int16_t sy;
    int16_t err;
    int16_t e2;
    int16_t ix0;
    int16_t iy0;
    int16_t ix1;
    int16_t iy1;

    if ((matrix == (ws2812_matrix_t*)0) || (matrix->initialized == 0u))
    {
        return 0u;
    }

    ix0 = (int16_t)x0;
    iy0 = (int16_t)y0;
    ix1 = (int16_t)x1;
    iy1 = (int16_t)y1;

    dx = (int16_t)(ix1 > ix0 ? (ix1 - ix0) : (ix0 - ix1));
    dy = (int16_t)(iy1 > iy0 ? (iy0 - iy1) : (iy1 - iy0));
    sx = (int16_t)(ix0 < ix1 ? 1 : -1);
    sy = (int16_t)(iy0 < iy1 ? 1 : -1);
    err = (int16_t)(dx + dy);

    while (1)
    {
        if ((ix0 >= 0) && (iy0 >= 0))
        {
            (void)ws2812_matrix_set_pixel(matrix, (uint16_t)ix0, (uint16_t)iy0, color);
        }

        if ((ix0 == ix1) && (iy0 == iy1))
        {
            break;
        }

        e2 = (int16_t)(err << 1);
        if (e2 >= dy)
        {
            err = (int16_t)(err + dy);
            ix0 = (int16_t)(ix0 + sx);
        }
        if (e2 <= dx)
        {
            err = (int16_t)(err + dx);
            iy0 = (int16_t)(iy0 + sy);
        }
    }

    return 1u;
}

uint8_t ws2812_matrix_draw_rect(ws2812_matrix_t* matrix,
                                uint16_t x,
                                uint16_t y,
                                uint16_t width,
                                uint16_t height,
                                rgb_color_t color,
                                uint8_t filled)
{
    uint16_t i;
    uint16_t j;

    if ((matrix == (ws2812_matrix_t*)0) || (matrix->initialized == 0u))
    {
        return 0u;
    }

    if ((width == 0u) || (height == 0u))
    {
        return 0u;
    }

    if (filled != 0u)
    {
        for (j = 0u; j < height; j++)
        {
            for (i = 0u; i < width; i++)
            {
                (void)ws2812_matrix_set_pixel(matrix, (uint16_t)(x + i), (uint16_t)(y + j), color);
            }
        }
        return 1u;
    }

    for (i = 0u; i < width; i++)
    {
        (void)ws2812_matrix_set_pixel(matrix, (uint16_t)(x + i), y, color);
        (void)ws2812_matrix_set_pixel(matrix, (uint16_t)(x + i), (uint16_t)(y + height - 1u), color);
    }

    for (j = 0u; j < height; j++)
    {
        (void)ws2812_matrix_set_pixel(matrix, x, (uint16_t)(y + j), color);
        (void)ws2812_matrix_set_pixel(matrix, (uint16_t)(x + width - 1u), (uint16_t)(y + j), color);
    }

    return 1u;
}
