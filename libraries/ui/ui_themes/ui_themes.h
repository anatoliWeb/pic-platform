/*
 * File: libraries/ui/ui_themes/ui_themes.h
 */

#ifndef LIBRARIES_UI_UI_THEMES_H
#define LIBRARIES_UI_UI_THEMES_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/ui/icon_pack/icon_pack.h"

typedef enum
{
    UI_COLOR_BACKGROUND = 0,
    UI_COLOR_FOREGROUND,
    UI_COLOR_ACCENT,
    UI_COLOR_WARNING,
    UI_COLOR_ERROR,
    UI_COLOR_SUCCESS,
    UI_COLOR_DISABLED,
    UI_COLOR_BORDER,
    UI_COLOR_COUNT
} ui_color_role_t;

typedef enum
{
    UI_ICON_STYLE_DEFAULT = 0,
    UI_ICON_STYLE_OUTLINE,
    UI_ICON_STYLE_FILLED
} ui_icon_style_t;

typedef struct
{
    uint16_t colors[UI_COLOR_COUNT];
    ui_icon_style_t icon_style;
    uint8_t monochrome;
} ui_theme_t;

const ui_theme_t* ui_theme_get_default(void);
const ui_theme_t* ui_theme_get_dark(void);
const ui_theme_t* ui_theme_get_light(void);

void ui_theme_apply(ui_theme_t* dst, const ui_theme_t* src);
uint16_t ui_theme_get_color(const ui_theme_t* theme, ui_color_role_t role);

void ui_theme_set_color(ui_theme_t* theme, ui_color_role_t role, uint16_t color);
ui_icon_style_t ui_theme_get_icon_style(const ui_theme_t* theme);

#endif /* LIBRARIES_UI_UI_THEMES_H */
