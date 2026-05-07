#ifndef LIBRARIES_UI_ICON_PACK_H
#define LIBRARIES_UI_ICON_PACK_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/bitmap/bitmap.h"
#include "libraries/display/graphics/graphics.h"

typedef enum
{
    UI_ICON_SETTINGS = 0,
    UI_ICON_WARNING,
    UI_ICON_ERROR,
    UI_ICON_INFO,

    UI_ICON_WIFI,
    UI_ICON_BLUETOOTH,
    UI_ICON_ETHERNET,

    UI_ICON_BATTERY,
    UI_ICON_CHARGING,
    UI_ICON_TEMPERATURE,
    UI_ICON_CLOCK,

    UI_ICON_PLAY,
    UI_ICON_STOP,
    UI_ICON_PAUSE,
    UI_ICON_MENU,
    UI_ICON_BACK,
    UI_ICON_ARROW_UP,
    UI_ICON_ARROW_DOWN,
    UI_ICON_ARROW_LEFT,
    UI_ICON_ARROW_RIGHT,

    UI_ICON_COUNT
} ui_icon_id_t;

typedef struct
{
    uint8_t width;
    uint8_t height;
    bitmap_format_t format;
    const uint8_t* data;
} ui_icon_t;

const ui_icon_t* icon_get(ui_icon_id_t id);

void icon_draw(graphics_t* g, ui_icon_id_t id, int16_t x, int16_t y, uint8_t color);
void icon_draw_scaled(graphics_t* g, ui_icon_id_t id, int16_t x, int16_t y, uint8_t scale, uint8_t color);
void icon_draw_inverted(graphics_t* g, ui_icon_id_t id, int16_t x, int16_t y, uint8_t color);

#endif /* LIBRARIES_UI_ICON_PACK_H */
