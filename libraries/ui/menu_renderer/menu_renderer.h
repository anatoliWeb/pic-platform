#ifndef LIBRARIES_UI_MENU_RENDERER_H
#define LIBRARIES_UI_MENU_RENDERER_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/ui/menu_core/menu_core.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/display_framework/display_framework.h"
#include "libraries/ui/icon_pack/icon_pack.h"
#include "libraries/ui/ui_themes/ui_themes.h"

typedef struct
{
    menu_t* menu;
    graphics_t* graphics;
    const ui_theme_t* theme;

    uint8_t show_icons;
    uint8_t show_status_bar;

    uint8_t line_height;
    uint8_t viewport_rows;
    uint8_t top_index;

    uint8_t x_padding;
    uint8_t y_padding;
} menu_renderer_t;

uint8_t menu_renderer_init(menu_renderer_t* renderer,
                           menu_t* menu,
                           graphics_t* graphics,
                           const ui_theme_t* theme);

void menu_renderer_draw(menu_renderer_t* renderer);

void menu_renderer_set_theme(menu_renderer_t* renderer, const ui_theme_t* theme);
void menu_renderer_set_icons(menu_renderer_t* renderer, uint8_t enabled);
void menu_renderer_set_layout(menu_renderer_t* renderer,
                              uint8_t line_height,
                              uint8_t viewport_rows,
                              uint8_t x_padding,
                              uint8_t y_padding);

#endif /* LIBRARIES_UI_MENU_RENDERER_H */
