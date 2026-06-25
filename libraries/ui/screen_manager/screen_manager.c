/*
 * File: libraries/ui/screen_manager/screen_manager.c
 */

#include "libraries/ui/screen_manager/screen_manager.h"

static screen_t* screen_manager_current(screen_manager_t* mgr)
{
    if ((mgr == (screen_manager_t*)0) || (mgr->top >= SCREEN_MANAGER_STACK_SIZE))
    {
        return (screen_t*)0;
    }

    return mgr->stack[mgr->top];
}

uint8_t screen_manager_init(screen_manager_t* mgr, graphics_t* graphics, ui_events_t* events)
{
    uint8_t i;

    if (mgr == (screen_manager_t*)0)
    {
        return 0u;
    }

    for (i = 0u; i < SCREEN_MANAGER_STACK_SIZE; i++)
    {
        mgr->stack[i] = (screen_t*)0;
    }

    mgr->top = 0u;
    mgr->graphics = graphics;
    mgr->events = events;

    return 1u;
}

uint8_t screen_manager_set(screen_manager_t* mgr, screen_t* screen)
{
    screen_t* cur;

    if ((mgr == (screen_manager_t*)0) || (screen == (screen_t*)0))
    {
        return 0u;
    }

    cur = screen_manager_current(mgr);
    if ((cur != (screen_t*)0) && (cur->on_exit != (screen_on_exit_cb_t)0))
    {
        cur->on_exit(cur, screen->user_data);
    }

    mgr->top = 0u;
    mgr->stack[0] = screen;

    if (screen->on_enter != (screen_on_enter_cb_t)0)
    {
        screen->on_enter(screen, (cur != (screen_t*)0) ? cur->user_data : (void*)0);
    }

    return 1u;
}

uint8_t screen_manager_push(screen_manager_t* mgr, screen_t* screen)
{
    screen_t* cur;

    if ((mgr == (screen_manager_t*)0) || (screen == (screen_t*)0))
    {
        return 0u;
    }

    if ((uint8_t)(mgr->top + 1u) >= SCREEN_MANAGER_STACK_SIZE)
    {
        return 0u;
    }

    cur = screen_manager_current(mgr);

    mgr->top++;
    mgr->stack[mgr->top] = screen;

    if (screen->on_enter != (screen_on_enter_cb_t)0)
    {
        screen->on_enter(screen, (cur != (screen_t*)0) ? cur->user_data : (void*)0);
    }

    return 1u;
}

uint8_t screen_manager_pop(screen_manager_t* mgr)
{
    screen_t* cur;
    screen_t* next;

    if ((mgr == (screen_manager_t*)0) || (mgr->top == 0u))
    {
        return 0u;
    }

    cur = screen_manager_current(mgr);
    next = mgr->stack[mgr->top - 1u];

    if ((cur != (screen_t*)0) && (cur->on_exit != (screen_on_exit_cb_t)0))
    {
        cur->on_exit(cur, (next != (screen_t*)0) ? next->user_data : (void*)0);
    }

    mgr->stack[mgr->top] = (screen_t*)0;
    mgr->top--;

    if ((next != (screen_t*)0) && (next->on_enter != (screen_on_enter_cb_t)0))
    {
        next->on_enter(next, (cur != (screen_t*)0) ? cur->user_data : (void*)0);
    }

    return 1u;
}

void screen_manager_send_event(screen_manager_t* mgr, const ui_event_t* event)
{
    screen_t* cur;

    if ((mgr == (screen_manager_t*)0) || (event == (const ui_event_t*)0))
    {
        return;
    }

    cur = screen_manager_current(mgr);
    if ((cur != (screen_t*)0) && (cur->on_event != (screen_on_event_cb_t)0))
    {
        cur->on_event(cur, event);
    }
}

void screen_manager_update(screen_manager_t* mgr)
{
    screen_t* cur;
    ui_event_t ev;

    if (mgr == (screen_manager_t*)0)
    {
        return;
    }

    cur = screen_manager_current(mgr);
    if ((cur != (screen_t*)0) && (cur->on_update != (screen_on_update_cb_t)0))
    {
        cur->on_update(cur);
    }

    if (mgr->events != (ui_events_t*)0)
    {
        while (ui_events_pop(mgr->events, &ev) != 0u)
        {
            screen_manager_send_event(mgr, &ev);
        }
    }
}

void screen_manager_draw(screen_manager_t* mgr)
{
    screen_t* cur;

    if (mgr == (screen_manager_t*)0)
    {
        return;
    }

    cur = screen_manager_current(mgr);
    if ((cur != (screen_t*)0) && (cur->on_draw != (screen_on_draw_cb_t)0))
    {
        cur->on_draw(cur, mgr->graphics);
    }
}

screen_t* screen_manager_get_current(screen_manager_t* mgr)
{
    return screen_manager_current(mgr);
}
