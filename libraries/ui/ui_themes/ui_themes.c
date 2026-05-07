#include "libraries/ui/ui_themes/ui_themes.h"

/*
 * Themes use generic 16-bit color values.
 * For monochrome displays, non-zero is treated as ON pixel.
 */
static const ui_theme_t g_theme_default =
{
    {
        0x0000u, /* background */
        0xFFFFu, /* foreground */
        0x07FFu, /* accent */
        0xFFE0u, /* warning */
        0xF800u, /* error */
        0x07E0u, /* success */
        0x8410u, /* disabled */
        0xFFFFu  /* border */
    },
    UI_ICON_STYLE_DEFAULT,
    0u
};

static const ui_theme_t g_theme_dark =
{
    {
        0x0000u, /* background */
        0xFFFFu, /* foreground */
        0x001Fu, /* accent */
        0xFD20u, /* warning */
        0xF800u, /* error */
        0x07E0u, /* success */
        0x4208u, /* disabled */
        0x7BEFu  /* border */
    },
    UI_ICON_STYLE_OUTLINE,
    0u
};

static const ui_theme_t g_theme_light =
{
    {
        0xFFFFu, /* background */
        0x0000u, /* foreground */
        0x001Fu, /* accent */
        0xFD20u, /* warning */
        0xC800u, /* error */
        0x0400u, /* success */
        0xBDF7u, /* disabled */
        0x0000u  /* border */
    },
    UI_ICON_STYLE_FILLED,
    0u
};

const ui_theme_t* ui_theme_get_default(void)
{
    return &g_theme_default;
}

const ui_theme_t* ui_theme_get_dark(void)
{
    return &g_theme_dark;
}

const ui_theme_t* ui_theme_get_light(void)
{
    return &g_theme_light;
}

void ui_theme_apply(ui_theme_t* dst, const ui_theme_t* src)
{
    uint8_t i;

    if ((dst == (ui_theme_t*)0) || (src == (const ui_theme_t*)0))
    {
        return;
    }

    for (i = 0u; i < (uint8_t)UI_COLOR_COUNT; i++)
    {
        dst->colors[i] = src->colors[i];
    }

    dst->icon_style = src->icon_style;
    dst->monochrome = src->monochrome;
}

uint16_t ui_theme_get_color(const ui_theme_t* theme, ui_color_role_t role)
{
    if ((theme == (const ui_theme_t*)0) || ((uint16_t)role >= (uint16_t)UI_COLOR_COUNT))
    {
        return 0u;
    }

    if (theme->monochrome != 0u)
    {
        return (uint16_t)(theme->colors[(uint8_t)role] != 0u ? 1u : 0u);
    }

    return theme->colors[(uint8_t)role];
}

void ui_theme_set_color(ui_theme_t* theme, ui_color_role_t role, uint16_t color)
{
    if ((theme == (ui_theme_t*)0) || ((uint16_t)role >= (uint16_t)UI_COLOR_COUNT))
    {
        return;
    }

    theme->colors[(uint8_t)role] = color;
}

ui_icon_style_t ui_theme_get_icon_style(const ui_theme_t* theme)
{
    if (theme == (const ui_theme_t*)0)
    {
        return UI_ICON_STYLE_DEFAULT;
    }

    return theme->icon_style;
}
