#include "libraries/ui/menu_navigation/menu_navigation.h"

uint8_t menu_navigation_init(menu_navigation_t* nav, menu_t* menu, input_adapter_t* input)
{
    if ((nav == (menu_navigation_t*)0) || (menu == (menu_t*)0) || (input == (input_adapter_t*)0))
    {
        return 0u;
    }

    nav->menu = menu;
    nav->input = input;
    nav->enabled = 1u;

    return 1u;
}

void menu_navigation_update(menu_navigation_t* nav)
{
    input_event_t ev;

    if ((nav == (menu_navigation_t*)0) || (nav->menu == (menu_t*)0) || (nav->input == (input_adapter_t*)0) || (nav->enabled == 0u))
    {
        return;
    }

    input_adapter_update(nav->input);

    while (1)
    {
        ev = input_adapter_get_event(nav->input);
        if (ev == INPUT_EVENT_NONE)
        {
            break;
        }

        switch (ev)
        {
            case INPUT_EVENT_UP:
                menu_prev(nav->menu);
                break;

            case INPUT_EVENT_DOWN:
                menu_next(nav->menu);
                break;

            case INPUT_EVENT_LEFT:
                /* Optional value handling through menu_core item action/value cycle. */
                menu_prev(nav->menu);
                break;

            case INPUT_EVENT_RIGHT:
                /* Optional value handling through menu_core item action/value cycle. */
                menu_next(nav->menu);
                break;

            case INPUT_EVENT_SELECT:
                menu_enter(nav->menu);
                break;

            case INPUT_EVENT_BACK:
                menu_back(nav->menu);
                break;

            case INPUT_EVENT_HOLD:
                /* Default hold behavior maps to BACK for quick exit. */
                menu_back(nav->menu);
                break;

            case INPUT_EVENT_NONE:
            default:
                break;
        }
    }
}

void menu_navigation_set_enabled(menu_navigation_t* nav, uint8_t enabled)
{
    if (nav == (menu_navigation_t*)0)
    {
        return;
    }

    nav->enabled = (uint8_t)(enabled != 0u ? 1u : 0u);
}

void menu_navigation_reset(menu_navigation_t* nav)
{
    if ((nav == (menu_navigation_t*)0) || (nav->menu == (menu_t*)0) || (nav->input == (input_adapter_t*)0))
    {
        return;
    }

    menu_reset(nav->menu);
    input_adapter_clear(nav->input);
}
