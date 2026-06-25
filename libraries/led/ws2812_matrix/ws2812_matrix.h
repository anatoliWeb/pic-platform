/*
 * File: libraries/led/ws2812_matrix/ws2812_matrix.h
 */

#ifndef LIBRARIES_LED_WS2812_MATRIX_H
#define LIBRARIES_LED_WS2812_MATRIX_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/led/ws2812/ws2812.h"
#include "libraries/led/rgb_utils/rgb_utils.h"

#define WS2812_MATRIX_INVALID_INDEX 0xFFFFu

typedef enum
{
    WS2812_MATRIX_LAYOUT_ROW_MAJOR = 0,
    WS2812_MATRIX_LAYOUT_COLUMN_MAJOR
} ws2812_matrix_layout_t;

typedef enum
{
    WS2812_MATRIX_ORIGIN_TOP_LEFT = 0,
    WS2812_MATRIX_ORIGIN_TOP_RIGHT,
    WS2812_MATRIX_ORIGIN_BOTTOM_LEFT,
    WS2812_MATRIX_ORIGIN_BOTTOM_RIGHT
} ws2812_matrix_origin_t;

typedef struct
{
    ws2812_t* strip;
    uint16_t width;
    uint16_t height;
    ws2812_matrix_layout_t layout;
    uint8_t serpentine;
    ws2812_matrix_origin_t origin;
    uint8_t reverse;
} ws2812_matrix_config_t;

typedef struct
{
    ws2812_t* strip;
    uint16_t width;
    uint16_t height;
    ws2812_matrix_layout_t layout;
    uint8_t serpentine;
    ws2812_matrix_origin_t origin;
    uint8_t reverse;
    uint8_t initialized;
} ws2812_matrix_t;

uint8_t ws2812_matrix_init(ws2812_matrix_t* matrix, const ws2812_matrix_config_t* config);

uint16_t ws2812_matrix_get_index(const ws2812_matrix_t* matrix, uint16_t x, uint16_t y);

uint8_t ws2812_matrix_set_pixel(ws2812_matrix_t* matrix, uint16_t x, uint16_t y, rgb_color_t color);

void ws2812_matrix_clear(ws2812_matrix_t* matrix);
void ws2812_matrix_fill(ws2812_matrix_t* matrix, rgb_color_t color);
void ws2812_matrix_show(ws2812_matrix_t* matrix);

uint8_t ws2812_matrix_draw_line(ws2812_matrix_t* matrix,
                                uint16_t x0,
                                uint16_t y0,
                                uint16_t x1,
                                uint16_t y1,
                                rgb_color_t color);

uint8_t ws2812_matrix_draw_rect(ws2812_matrix_t* matrix,
                                uint16_t x,
                                uint16_t y,
                                uint16_t width,
                                uint16_t height,
                                rgb_color_t color,
                                uint8_t filled);

#endif /* LIBRARIES_LED_WS2812_MATRIX_H */
