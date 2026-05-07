#include "libraries/ui/icon_pack/icon_pack.h"

/* 8x8 monochrome 1bpp icons, row-based, MSB-first. */
static const uint8_t g_icon_settings[]    = {0x18,0x3C,0x5A,0xE7,0xE7,0x5A,0x3C,0x18};
static const uint8_t g_icon_warning[]     = {0x18,0x3C,0x3C,0x7E,0x7E,0x18,0x00,0x18};
static const uint8_t g_icon_error[]       = {0x81,0x42,0x24,0x18,0x18,0x24,0x42,0x81};
static const uint8_t g_icon_info[]        = {0x3C,0x42,0x18,0x18,0x18,0x00,0x18,0x00};

static const uint8_t g_icon_wifi[]        = {0x00,0x3C,0x42,0x18,0x24,0x00,0x18,0x00};
static const uint8_t g_icon_bluetooth[]   = {0x10,0x18,0x14,0x12,0x14,0x18,0x10,0x00};
static const uint8_t g_icon_ethernet[]    = {0x7E,0x42,0x7E,0x52,0x7E,0x12,0x1E,0x00};

static const uint8_t g_icon_battery[]     = {0x00,0x7E,0x42,0x42,0x42,0x42,0x7E,0x18};
static const uint8_t g_icon_charging[]    = {0x08,0x1C,0x18,0x3C,0x0C,0x1C,0x08,0x00};
static const uint8_t g_icon_temperature[] = {0x18,0x24,0x24,0x24,0x24,0x66,0x66,0x3C};
static const uint8_t g_icon_clock[]       = {0x3C,0x42,0x46,0x4A,0x52,0x42,0x3C,0x00};

static const uint8_t g_icon_play[]        = {0x40,0x60,0x70,0x78,0x78,0x70,0x60,0x40};
static const uint8_t g_icon_stop[]        = {0x00,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x00};
static const uint8_t g_icon_pause[]       = {0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x00};
static const uint8_t g_icon_menu[]        = {0x00,0x7E,0x00,0x7E,0x00,0x7E,0x00,0x00};
static const uint8_t g_icon_back[]        = {0x10,0x18,0x1C,0xFE,0x1C,0x18,0x10,0x00};
static const uint8_t g_icon_arrow_up[]    = {0x18,0x3C,0x66,0x18,0x18,0x18,0x18,0x00};
static const uint8_t g_icon_arrow_down[]  = {0x18,0x18,0x18,0x18,0x66,0x3C,0x18,0x00};
static const uint8_t g_icon_arrow_left[]  = {0x10,0x18,0x1C,0xFE,0x1C,0x18,0x10,0x00};
static const uint8_t g_icon_arrow_right[] = {0x08,0x18,0x38,0xFE,0x38,0x18,0x08,0x00};

static const ui_icon_t g_icons[UI_ICON_COUNT] =
{
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_settings},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_warning},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_error},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_info},

    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_wifi},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_bluetooth},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_ethernet},

    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_battery},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_charging},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_temperature},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_clock},

    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_play},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_stop},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_pause},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_menu},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_back},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_arrow_up},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_arrow_down},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_arrow_left},
    {8u, 8u, BITMAP_FORMAT_1BPP_ROW, g_icon_arrow_right}
};

const ui_icon_t* icon_get(ui_icon_id_t id)
{
    if ((uint16_t)id >= (uint16_t)UI_ICON_COUNT)
    {
        return (const ui_icon_t*)0;
    }

    return &g_icons[(uint8_t)id];
}

static void icon_to_bitmap(const ui_icon_t* icon, bitmap_t* bmp)
{
    if ((icon == (const ui_icon_t*)0) || (bmp == (bitmap_t*)0))
    {
        return;
    }

    bmp->width = icon->width;
    bmp->height = icon->height;
    bmp->format = icon->format;
    bmp->data = icon->data;
}

void icon_draw(graphics_t* g, ui_icon_id_t id, int16_t x, int16_t y, uint8_t color)
{
    const ui_icon_t* icon;
    bitmap_t bmp;

    icon = icon_get(id);
    if ((g == (graphics_t*)0) || (icon == (const ui_icon_t*)0))
    {
        return;
    }

    icon_to_bitmap(icon, &bmp);
    bitmap_draw_transparent(g, x, y, &bmp, color, 0u);
}

void icon_draw_scaled(graphics_t* g, ui_icon_id_t id, int16_t x, int16_t y, uint8_t scale, uint8_t color)
{
    const ui_icon_t* icon;
    bitmap_t bmp;

    icon = icon_get(id);
    if ((g == (graphics_t*)0) || (icon == (const ui_icon_t*)0))
    {
        return;
    }

    icon_to_bitmap(icon, &bmp);
    bitmap_draw_scaled(g, x, y, &bmp, scale, color);
}

void icon_draw_inverted(graphics_t* g, ui_icon_id_t id, int16_t x, int16_t y, uint8_t color)
{
    const ui_icon_t* icon;
    bitmap_t bmp;

    icon = icon_get(id);
    if ((g == (graphics_t*)0) || (icon == (const ui_icon_t*)0))
    {
        return;
    }

    icon_to_bitmap(icon, &bmp);
    bitmap_draw_inverted(g, x, y, &bmp, color);
}

