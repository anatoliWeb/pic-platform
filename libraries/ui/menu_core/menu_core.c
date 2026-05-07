#include "libraries/ui/menu_core/menu_core.h"

static menu_item_t* menu_first_child_or_self(menu_item_t* item)
{
    if (item == (menu_item_t*)0)
    {
        return (menu_item_t*)0;
    }

    if (item->first_child != (menu_item_t*)0)
    {
        return item->first_child;
    }

    return item;
}

static void menu_apply_item_action(menu_item_t* item)
{
    if (item == (menu_item_t*)0)
    {
        return;
    }

    switch (item->type)
    {
        case MENU_ITEM_TOGGLE:
            item->toggle_value = (uint8_t)(item->toggle_value == 0u ? 1u : 0u);
            break;

        case MENU_ITEM_VALUE:
            if (item->step == 0)
            {
                item->step = 1;
            }

            item->value = (int16_t)(item->value + item->step);
            if (item->value > item->max_value)
            {
                item->value = item->min_value;
            }
            break;

        case MENU_ITEM_ACTION:
        case MENU_ITEM_SUBMENU:
        default:
            break;
    }

    if (item->action != (menu_action_cb_t)0)
    {
        item->action(item->action_ctx);
    }
}

void menu_init(menu_t* menu)
{
    if (menu == (menu_t*)0)
    {
        return;
    }

    menu->root = (menu_item_t*)0;
    menu->current = (menu_item_t*)0;
    menu->selected = (menu_item_t*)0;
}

void menu_set_root(menu_t* menu, menu_item_t* root)
{
    if (menu == (menu_t*)0)
    {
        return;
    }

    menu->root = root;
    menu->current = root;
    menu->selected = menu_first_child_or_self(root);
}

menu_item_t* menu_get_current(menu_t* menu)
{
    if (menu == (menu_t*)0)
    {
        return (menu_item_t*)0;
    }

    return menu->selected;
}

void menu_next(menu_t* menu)
{
    if ((menu == (menu_t*)0) || (menu->selected == (menu_item_t*)0))
    {
        return;
    }

    if (menu->selected->next != (menu_item_t*)0)
    {
        menu->selected = menu->selected->next;
    }
}

void menu_prev(menu_t* menu)
{
    if ((menu == (menu_t*)0) || (menu->selected == (menu_item_t*)0))
    {
        return;
    }

    if (menu->selected->prev != (menu_item_t*)0)
    {
        menu->selected = menu->selected->prev;
    }
}

void menu_enter(menu_t* menu)
{
    menu_item_t* item;

    if ((menu == (menu_t*)0) || (menu->selected == (menu_item_t*)0))
    {
        return;
    }

    item = menu->selected;

    if ((item->type == MENU_ITEM_SUBMENU) && (item->first_child != (menu_item_t*)0))
    {
        menu->current = item;
        menu->selected = item->first_child;
        return;
    }

    menu_apply_item_action(item);
}

void menu_back(menu_t* menu)
{
    if ((menu == (menu_t*)0) || (menu->current == (menu_item_t*)0))
    {
        return;
    }

    if (menu->current->parent != (menu_item_t*)0)
    {
        menu->selected = menu->current;
        menu->current = menu->current->parent;
    }
}

void menu_select(menu_t* menu, menu_item_t* item)
{
    if ((menu == (menu_t*)0) || (item == (menu_item_t*)0))
    {
        return;
    }

    menu->selected = item;
}

void menu_reset(menu_t* menu)
{
    if ((menu == (menu_t*)0) || (menu->root == (menu_item_t*)0))
    {
        return;
    }

    menu->current = menu->root;
    menu->selected = menu_first_child_or_self(menu->root);
}
