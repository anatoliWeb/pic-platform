#include "libraries/ui/menu_widgets/menu_widgets.h"

static uint8_t widget_color(const ui_theme_t* theme, ui_color_role_t role)
{
    return (uint8_t)(ui_theme_get_color(theme, role) != 0u ? 1u : 0u);
}

static int16_t widget_clamp(int16_t v, int16_t lo, int16_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static void draw_checkbox(menu_widget_t* w, graphics_t* g, const ui_theme_t* t, uint8_t selected)
{
    uint8_t fg;
    fg = widget_color(t, selected != 0u ? UI_COLOR_ACCENT : UI_COLOR_FOREGROUND);

    graphics_draw_rect(g, w->x, w->y, w->width, w->height, fg);

    if ((w->value_ptr != (int16_t*)0) && (*w->value_ptr != 0))
    {
        graphics_draw_line(g, (int16_t)(w->x + 2), (int16_t)(w->y + w->height / 2), (int16_t)(w->x + w->width - 3), (int16_t)(w->y + w->height / 2), fg);
        graphics_draw_line(g, (int16_t)(w->x + w->width / 2), (int16_t)(w->y + 2), (int16_t)(w->x + w->width / 2), (int16_t)(w->y + w->height - 3), fg);
    }
}

static void draw_toggle(menu_widget_t* w, graphics_t* g, const ui_theme_t* t, uint8_t selected)
{
    uint8_t fg;
    uint8_t bg;

    fg = widget_color(t, selected != 0u ? UI_COLOR_ACCENT : UI_COLOR_FOREGROUND);
    bg = widget_color(t, UI_COLOR_BACKGROUND);

    graphics_draw_rect(g, w->x, w->y, w->width, w->height, fg);

    if ((w->value_ptr != (int16_t*)0) && (*w->value_ptr != 0))
    {
        graphics_fill_rect(g, (int16_t)(w->x + w->width / 2), (int16_t)(w->y + 1), (int16_t)(w->width / 2 - 1), (int16_t)(w->height - 2), fg);
    }
    else
    {
        graphics_fill_rect(g, (int16_t)(w->x + 1), (int16_t)(w->y + 1), (int16_t)(w->width / 2 - 1), (int16_t)(w->height - 2), bg);
    }
}

static void draw_progress(menu_widget_t* w, graphics_t* g, const ui_theme_t* t, uint8_t selected)
{
    int16_t v;
    int16_t span;
    int16_t filled;
    uint8_t fg;
    uint8_t ac;

    fg = widget_color(t, selected != 0u ? UI_COLOR_ACCENT : UI_COLOR_FOREGROUND);
    ac = widget_color(t, UI_COLOR_SUCCESS);

    graphics_draw_rect(g, w->x, w->y, w->width, w->height, fg);

    if (w->value_ptr == (int16_t*)0)
    {
        return;
    }

    v = widget_clamp(*w->value_ptr, w->min_value, w->max_value);
    span = (int16_t)(w->max_value - w->min_value);
    if (span <= 0)
    {
        span = 1;
    }

    filled = (int16_t)(((int32_t)(v - w->min_value) * (w->width - 2)) / span);
    if (filled < 0) filled = 0;
    if (filled > (w->width - 2)) filled = (int16_t)(w->width - 2);

    graphics_fill_rect(g, (int16_t)(w->x + 1), (int16_t)(w->y + 1), filled, (int16_t)(w->height - 2), ac);
}

static void draw_numeric(menu_widget_t* w, graphics_t* g, const ui_theme_t* t, uint8_t selected)
{
    char buf[8];
    int16_t v;
    int8_t pos;
    uint8_t fg;

    fg = widget_color(t, selected != 0u ? UI_COLOR_ACCENT : UI_COLOR_FOREGROUND);
    graphics_draw_rect(g, w->x, w->y, w->width, w->height, fg);

    if (w->value_ptr == (int16_t*)0)
    {
        return;
    }

    v = *w->value_ptr;
    buf[7] = '\0';
    pos = 6;

    if (v == 0)
    {
        buf[pos--] = '0';
    }
    else
    {
        int16_t tval;
        uint8_t neg;

        neg = 0u;
        tval = v;
        if (tval < 0)
        {
            neg = 1u;
            tval = (int16_t)(-tval);
        }

        while ((tval > 0) && (pos >= 0))
        {
            buf[pos--] = (char)('0' + (tval % 10));
            tval = (int16_t)(tval / 10);
        }

        if ((neg != 0u) && (pos >= 0))
        {
            buf[pos--] = '-';
        }
    }

    graphics_draw_string(g, (int16_t)(w->x + 2), (int16_t)(w->y + 1), &buf[pos + 1], fg);
}

static void draw_option(menu_widget_t* w, graphics_t* g, const ui_theme_t* t, uint8_t selected)
{
    uint8_t fg;
    int16_t idx;

    fg = widget_color(t, selected != 0u ? UI_COLOR_ACCENT : UI_COLOR_FOREGROUND);

    graphics_draw_rect(g, w->x, w->y, w->width, w->height, fg);

    if ((w->value_ptr == (int16_t*)0) || (w->options == (const char**)0) || (w->option_count == 0u))
    {
        return;
    }

    idx = *w->value_ptr;
    if (idx < 0)
    {
        idx = 0;
    }
    if (idx >= (int16_t)w->option_count)
    {
        idx = (int16_t)(w->option_count - 1u);
    }

    graphics_draw_string(g, (int16_t)(w->x + 2), (int16_t)(w->y + 1), w->options[idx], fg);
}

void menu_widget_init(menu_widget_t* widget,
                      menu_widget_type_t type,
                      int16_t x,
                      int16_t y,
                      int16_t width,
                      int16_t height,
                      int16_t* value_ptr,
                      int16_t min_value,
                      int16_t max_value,
                      int16_t step)
{
    if (widget == (menu_widget_t*)0)
    {
        return;
    }

    widget->type = type;
    widget->x = x;
    widget->y = y;
    widget->width = width;
    widget->height = height;

    widget->color_bg = 0u;
    widget->color_fg = 1u;
    widget->color_accent = 1u;

    widget->value_ptr = value_ptr;
    widget->min_value = min_value;
    widget->max_value = max_value;
    widget->step = (step == 0) ? 1 : step;

    widget->options = (const char**)0;
    widget->option_count = 0u;

    widget->menu_item = (menu_item_t*)0;

    widget->draw_cb = (menu_widget_draw_cb_t)0;
    widget->update_cb = (menu_widget_update_cb_t)0;
}

void menu_widget_draw(menu_widget_t* widget,
                      graphics_t* g,
                      const ui_theme_t* theme,
                      uint8_t selected)
{
    if ((widget == (menu_widget_t*)0) || (g == (graphics_t*)0) || (theme == (const ui_theme_t*)0))
    {
        return;
    }

    if (widget->draw_cb != (menu_widget_draw_cb_t)0)
    {
        widget->draw_cb(widget, g, theme, selected);
        return;
    }

    switch (widget->type)
    {
        case MENU_WIDGET_CHECKBOX:
            draw_checkbox(widget, g, theme, selected);
            break;

        case MENU_WIDGET_TOGGLE:
            draw_toggle(widget, g, theme, selected);
            break;

        case MENU_WIDGET_PROGRESS:
        case MENU_WIDGET_SLIDER:
            draw_progress(widget, g, theme, selected);
            break;

        case MENU_WIDGET_NUMERIC:
            draw_numeric(widget, g, theme, selected);
            break;

        case MENU_WIDGET_OPTION:
            draw_option(widget, g, theme, selected);
            break;

        case MENU_WIDGET_TEXT:
        case MENU_WIDGET_GRAPH:
        default:
            graphics_draw_rect(g, widget->x, widget->y, widget->width, widget->height, widget_color(theme, UI_COLOR_FOREGROUND));
            break;
    }
}

void menu_widget_update(menu_widget_t* widget)
{
    if (widget == (menu_widget_t*)0)
    {
        return;
    }

    if (widget->update_cb != (menu_widget_update_cb_t)0)
    {
        widget->update_cb(widget);
    }
}

void menu_widget_bind_menu_item(menu_widget_t* widget, menu_item_t* item)
{
    if (widget == (menu_widget_t*)0)
    {
        return;
    }

    widget->menu_item = item;
}
