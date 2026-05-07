#ifndef LIBRARIES_UI_SCREEN_TRANSITIONS_H
#define LIBRARIES_UI_SCREEN_TRANSITIONS_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/ui/screen_manager/screen_manager.h"
#include "libraries/ui/overlay_system/overlay_system.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/ui/sprite_buffer/sprite_buffer.h"

typedef enum
{
    SCREEN_TRANSITION_FADE = 0,
    SCREEN_TRANSITION_SLIDE_LEFT,
    SCREEN_TRANSITION_SLIDE_RIGHT,
    SCREEN_TRANSITION_SLIDE_UP,
    SCREEN_TRANSITION_SLIDE_DOWN
} screen_transition_type_t;

typedef struct
{
    screen_transition_type_t type;
    uint32_t duration_ms;
    uint16_t progress;
    uint8_t active;

    uint32_t started_ms;

    screen_t* source_screen;
    screen_t* target_screen;

    void* user_data;
} transition_t;

typedef struct
{
    transition_t transition;

    screen_manager_t* screens;
    overlay_system_t* overlays;
    graphics_t* graphics;

    uint16_t speed_percent;
} screen_transition_manager_t;

uint8_t screen_transition_init(screen_transition_manager_t* mgr,
                               screen_manager_t* screens,
                               overlay_system_t* overlays,
                               graphics_t* graphics);

uint8_t screen_transition_start(screen_transition_manager_t* mgr,
                                screen_transition_type_t type,
                                uint32_t duration_ms,
                                screen_t* target_screen,
                                void* user_data,
                                uint32_t now_ms);

void screen_transition_update(screen_transition_manager_t* mgr, uint32_t now_ms);
void screen_transition_draw(screen_transition_manager_t* mgr);
uint8_t screen_transition_is_active(const screen_transition_manager_t* mgr);

void screen_transition_cancel(screen_transition_manager_t* mgr);
void screen_transition_set_speed(screen_transition_manager_t* mgr, uint16_t speed_percent);

#endif /* LIBRARIES_UI_SCREEN_TRANSITIONS_H */
