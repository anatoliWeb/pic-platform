#include "libraries/ui/menu_renderer/menu_renderer.h"

static menu_item_t* menu_renderer_first_visible(const menu_t* menu)
{
    if ((menu == (const menu_t*)0) || (menu->current == (menu_item_t*)0))
    {
        return (menu_item_t*)0;
    }

    if (menu->current->first_child != (menu_item_t*)0)
    {
        return menu->current->first_child;
    }

    return menu->current;
}

static uint8_t menu_renderer_count_items(menu_item_t* first)
{
    uint8_t n;

    n = 0u;
    while (first != (menu_item_t*)0)
    {
        n++;
        first = first->next;
    }

    return n;
}

static uint8_t menu_renderer_selected_index(menu_item_t* first, const menu_item_t* selected)
{
    uint8_t idx;

    idx = 0u;
    while (first != (menu_item_t*)0)
    {
        if (first == selected)
        {
            return idx;
        }
        idx++;
        first = first->next;
    }

    return 0u;
}

static menu_item_t* menu_renderer_item_at(menu_item_t* first, uint8_t index)
{
    while ((first != (menu_item_t*)0) && (index > 0u))
    {
        first = first->next;
        index--;
    }

    return first;
}

static uint8_t menu_renderer_mono_color(const ui_theme_t* theme, ui_color_role_t role)
{
    if (ui_theme_get_color(theme, role) != 0u)
    {
        return 1u;
    }

    return 0u;
}

static void menu_renderer_draw_item_line(menu_renderer_t* renderer,
                                         menu_item_t* item,
                                         uint8_t row,
                                         uint8_t selected)
{
    uint8_t y;
    uint8_t bg;
    uint8_t fg;
    uint8_t x;

    y = (uint8_t)(renderer->y_padding + (row * renderer->line_height));
    bg = menu_renderer_mono_color(renderer->theme, UI_COLOR_BACKGROUND);
    fg = menu_renderer_mono_color(renderer->theme, UI_COLOR_FOREGROUND);

    if (selected != 0u)
    {
        /* Invert colors for selected row highlight. */
        uint8_t tmp;
        tmp = bg;
        bg = fg;
        fg = tmp;
    }

    graphics_fill_rect(renderer->graphics,
                       0,
                       y,
                       (int16_t)renderer->graphics->width,
                       renderer->line_height,
                       bg);

    x = renderer->x_padding;

    if ((renderer->show_icons != 0u) && (item != (menu_item_t*)0))
    {
        ui_icon_id_t icon_id;

        /* Simple icon mapping by item type. */
        if (item->type == MENU_ITEM_SUBMENU)
        {
            icon_id = UI_ICON_MENU;
        }
        else if (item->type == MENU_ITEM_TOGGLE)
        {
            icon_id = (item->toggle_value != 0u) ? UI_ICON_PLAY : UI_ICON_STOP;
        }
        else if (item->type == MENU_ITEM_VALUE)
        {
            icon_id = UI_ICON_SETTINGS;
        }
        else
        {
            icon_id = UI_ICON_INFO;
        }

        icon_draw(renderer->graphics, icon_id, x, y, fg);
        x = (uint8_t)(x + 10u);
    }

    if ((item != (menu_item_t*)0) && (item->label != (const char*)0))
    {
        graphics_draw_string(renderer->graphics, x, y, item->label, fg);
    }

    if (item != (menu_item_t*)0)
    {
        /* Draw right-side indicator for submenu/toggle/value. */
        if (item->type == MENU_ITEM_SUBMENU)
        {
            icon_draw(renderer->graphics,
                      UI_ICON_ARROW_RIGHT,
                      (int16_t)(renderer->graphics->width - 10u),
                      y,
                      fg);
        }
        else if (item->type == MENU_ITEM_TOGGLE)
        {
            if (item->toggle_value != 0u)
            {
                icon_draw(renderer->graphics,
                          UI_ICON_PLAY,
                          (int16_t)(renderer->graphics->width - 10u),
                          y,
                          fg);
            }
        }
        else if (item->type == MENU_ITEM_VALUE)
        {
            int16_t vx;
            int16_t vy;
            int16_t v;
            char s[7];
            int8_t pos;

            vx = (int16_t)(renderer->graphics->width - 24u);
            vy = y;
            v = item->value;

            s[6] = '\0';
            pos = 5;
            if (v == 0)
            {
                s[pos--] = '0';
            }
            else
            {
                int16_t t;
                uint8_t neg;
                neg = 0u;
                t = v;
                if (t < 0)
                {
                    neg = 1u;
                    t = (int16_t)(-t);
                }

                while ((t > 0) && (pos >= 0))
                {
                    s[pos--] = (char)('0' + (t % 10));
                    t = (int16_t)(t / 10);
                }

                if ((neg != 0u) && (pos >= 0))
                {
                    s[pos--] = '-';
                }
            }

            graphics_draw_string(renderer->graphics, vx, vy, &s[pos + 1], fg);
        }
    }
}

uint8_t menu_renderer_init(menu_renderer_t* renderer,
                           menu_t* menu,
                           graphics_t* graphics,
                           const ui_theme_t* theme)
{
    if ((renderer == (menu_renderer_t*)0) ||
        (menu == (menu_t*)0) ||
        (graphics == (graphics_t*)0) ||
        (theme == (const ui_theme_t*)0))
    {
        return 0u;
    }

    renderer->menu = menu;
    renderer->graphics = graphics;
    renderer->theme = theme;

    renderer->show_icons = 1u;
    renderer->show_status_bar = 0u;

    renderer->line_height = 10u;
    renderer->viewport_rows = (uint8_t)(graphics->height / renderer->line_height);
    if (renderer->viewport_rows == 0u)
    {
        renderer->viewport_rows = 1u;
    }

    renderer->top_index = 0u;
    renderer->x_padding = 2u;
    renderer->y_padding = 0u;

    return 1u;
}

void menu_renderer_draw(menu_renderer_t* renderer)
{
    menu_item_t* first;
    menu_item_t* item;
    menu_item_t* selected;
    uint8_t count;
    uint8_t sel_idx;
    uint8_t row;

    if ((renderer == (menu_renderer_t*)0) ||
        (renderer->menu == (menu_t*)0) ||
        (renderer->graphics == (graphics_t*)0) ||
        (renderer->theme == (const ui_theme_t*)0))
    {
        return;
    }

    first = menu_renderer_first_visible(renderer->menu);
    selected = renderer->menu->selected;

    if (first == (menu_item_t*)0)
    {
        return;
    }

    count = menu_renderer_count_items(first);
    sel_idx = menu_renderer_selected_index(first, selected);

    if (sel_idx < renderer->top_index)
    {
        renderer->top_index = sel_idx;
    }
    else if (sel_idx >= (uint8_t)(renderer->top_index + renderer->viewport_rows))
    {
        renderer->top_index = (uint8_t)(sel_idx - renderer->viewport_rows + 1u);
    }

    if (renderer->top_index >= count)
    {
        renderer->top_index = 0u;
    }

    /* Clear full viewport area. */
    graphics_fill_rect(renderer->graphics,
                       0,
                       0,
                       (int16_t)renderer->graphics->width,
                       (int16_t)renderer->graphics->height,
                       menu_renderer_mono_color(renderer->theme, UI_COLOR_BACKGROUND));

    item = menu_renderer_item_at(first, renderer->top_index);

    row = 0u;
    while ((item != (menu_item_t*)0) && (row < renderer->viewport_rows))
    {
        menu_renderer_draw_item_line(renderer, item, row, (uint8_t)(item == selected ? 1u : 0u));
        row++;
        item = item->next;
    }

    if (renderer->show_status_bar != 0u)
    {
        uint8_t by;
        by = (uint8_t)(renderer->graphics->height - 2u);
        graphics_draw_line(renderer->graphics,
                           0,
                           by,
                           (int16_t)(renderer->graphics->width - 1u),
                           by,
                           menu_renderer_mono_color(renderer->theme, UI_COLOR_BORDER));
    }
}

void menu_renderer_set_theme(menu_renderer_t* renderer, const ui_theme_t* theme)
{
    if ((renderer == (menu_renderer_t*)0) || (theme == (const ui_theme_t*)0))
    {
        return;
    }

    renderer->theme = theme;
}

void menu_renderer_set_icons(menu_renderer_t* renderer, uint8_t enabled)
{
    if (renderer == (menu_renderer_t*)0)
    {
        return;
    }

    renderer->show_icons = (uint8_t)(enabled != 0u ? 1u : 0u);
}

void menu_renderer_set_layout(menu_renderer_t* renderer,
                              uint8_t line_height,
                              uint8_t viewport_rows,
                              uint8_t x_padding,
                              uint8_t y_padding)
{
    if ((renderer == (menu_renderer_t*)0) || (line_height == 0u))
    {
        return;
    }

    renderer->line_height = line_height;

    if (viewport_rows == 0u)
    {
        renderer->viewport_rows = (uint8_t)(renderer->graphics->height / line_height);
        if (renderer->viewport_rows == 0u)
        {
            renderer->viewport_rows = 1u;
        }
    }
    else
    {
        renderer->viewport_rows = viewport_rows;
    }

    renderer->x_padding = x_padding;
    renderer->y_padding = y_padding;
}
