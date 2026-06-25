/*
 * File: libraries/ui/menu_widgets/menu_widgets.h
 */

#ifndef LIBRARIES_UI_MENU_WIDGETS_H
#define LIBRARIES_UI_MENU_WIDGETS_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/ui/menu_core/menu_core.h"
#include "libraries/ui/menu_renderer/menu_renderer.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/ui/ui_themes/ui_themes.h"

typedef enum
{
    MENU_WIDGET_CHECKBOX = 0,
    MENU_WIDGET_TOGGLE,
    MENU_WIDGET_PROGRESS,
    MENU_WIDGET_NUMERIC,
    MENU_WIDGET_OPTION,
    MENU_WIDGET_SLIDER,
    MENU_WIDGET_TEXT,
    MENU_WIDGET_GRAPH
} menu_widget_type_t;

typedef struct menu_widget_s menu_widget_t;

typedef void (*menu_widget_draw_cb_t)(menu_widget_t* widget,
                                      graphics_t* g,
                                      const ui_theme_t* theme,
                                      uint8_t selected);
typedef void (*menu_widget_update_cb_t)(menu_widget_t* widget);

struct menu_widget_s
{
    menu_widget_type_t type;

    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;

    uint16_t color_bg;
    uint16_t color_fg;
    uint16_t color_accent;

    int16_t* value_ptr;
    int16_t min_value;
    int16_t max_value;
    int16_t step;

    const char** options;
    uint8_t option_count;

    menu_item_t* menu_item;

    menu_widget_draw_cb_t draw_cb;
    menu_widget_update_cb_t update_cb;
};

void menu_widget_init(menu_widget_t* widget,
                      menu_widget_type_t type,
                      int16_t x,
                      int16_t y,
                      int16_t width,
                      int16_t height,
                      int16_t* value_ptr,
                      int16_t min_value,
                      int16_t max_value,
                      int16_t step);

void menu_widget_draw(menu_widget_t* widget,
                      graphics_t* g,
                      const ui_theme_t* theme,
                      uint8_t selected);

void menu_widget_update(menu_widget_t* widget);

void menu_widget_bind_menu_item(menu_widget_t* widget, menu_item_t* item);

#endif /* LIBRARIES_UI_MENU_WIDGETS_H */
