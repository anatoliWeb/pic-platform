/*
 * File: libraries/ui/screen_manager/screen_manager.h
 */

#ifndef LIBRARIES_UI_SCREEN_MANAGER_H
#define LIBRARIES_UI_SCREEN_MANAGER_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/ui/ui_events/ui_events.h"
#include "libraries/ui/menu_renderer/menu_renderer.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/display_framework/display_framework.h"

#define SCREEN_MANAGER_STACK_SIZE 8u

typedef struct screen_s screen_t;

typedef void (*screen_on_enter_cb_t)(screen_t* screen, void* prev_user_data);
typedef void (*screen_on_exit_cb_t)(screen_t* screen, void* next_user_data);
typedef void (*screen_on_update_cb_t)(screen_t* screen);
typedef void (*screen_on_draw_cb_t)(screen_t* screen, graphics_t* g);
typedef void (*screen_on_event_cb_t)(screen_t* screen, const ui_event_t* event);

struct screen_s
{
    uint8_t id;
    void* user_data;

    screen_on_enter_cb_t on_enter;
    screen_on_exit_cb_t on_exit;
    screen_on_update_cb_t on_update;
    screen_on_draw_cb_t on_draw;
    screen_on_event_cb_t on_event;
};

typedef struct
{
    screen_t* stack[SCREEN_MANAGER_STACK_SIZE];
    uint8_t top;

    graphics_t* graphics;
    ui_events_t* events;
} screen_manager_t;

uint8_t screen_manager_init(screen_manager_t* mgr, graphics_t* graphics, ui_events_t* events);

uint8_t screen_manager_set(screen_manager_t* mgr, screen_t* screen);
uint8_t screen_manager_push(screen_manager_t* mgr, screen_t* screen);
uint8_t screen_manager_pop(screen_manager_t* mgr);

void screen_manager_update(screen_manager_t* mgr);
void screen_manager_draw(screen_manager_t* mgr);

void screen_manager_send_event(screen_manager_t* mgr, const ui_event_t* event);
screen_t* screen_manager_get_current(screen_manager_t* mgr);

#endif /* LIBRARIES_UI_SCREEN_MANAGER_H */
