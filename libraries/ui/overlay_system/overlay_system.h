#ifndef LIBRARIES_UI_OVERLAY_SYSTEM_H
#define LIBRARIES_UI_OVERLAY_SYSTEM_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/ui/ui_events/ui_events.h"
#include "libraries/ui/screen_manager/screen_manager.h"
#include "libraries/ui/popup_manager/popup_manager.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/ui/ui_themes/ui_themes.h"
#include "libraries/ui/icon_pack/icon_pack.h"
#include "libraries/ui/sprite_buffer/sprite_buffer.h"

#define OVERLAY_SYSTEM_MAX_OVERLAYS 8u

typedef struct overlay_s overlay_t;

typedef void (*overlay_on_update_cb_t)(overlay_t* overlay, uint32_t now_ms);
typedef void (*overlay_on_draw_cb_t)(overlay_t* overlay, graphics_t* graphics, const ui_theme_t* theme);
typedef void (*overlay_on_event_cb_t)(overlay_t* overlay, const ui_event_t* event);

struct overlay_s
{
    uint8_t id;
    uint8_t visible;
    int16_t z_index;

    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;

    void* user_data;

    overlay_on_update_cb_t on_update;
    overlay_on_draw_cb_t on_draw;
    overlay_on_event_cb_t on_event;
};

typedef struct
{
    overlay_t* overlays[OVERLAY_SYSTEM_MAX_OVERLAYS];
    uint8_t count;

    graphics_t* graphics;
    const ui_theme_t* theme;
    ui_events_t* events;
    screen_manager_t* screens;
    popup_manager_t* popups;
} overlay_system_t;

uint8_t overlay_system_init(overlay_system_t* sys,
                            graphics_t* graphics,
                            const ui_theme_t* theme,
                            ui_events_t* events,
                            screen_manager_t* screens,
                            popup_manager_t* popups);

uint8_t overlay_add(overlay_system_t* sys, overlay_t* overlay);
uint8_t overlay_remove(overlay_system_t* sys, uint8_t overlay_id);

void overlay_update(overlay_system_t* sys, uint32_t now_ms);
void overlay_draw(overlay_system_t* sys);

uint8_t overlay_set_visible(overlay_system_t* sys, uint8_t overlay_id, uint8_t visible);
uint8_t overlay_bring_to_front(overlay_system_t* sys, uint8_t overlay_id);

#endif /* LIBRARIES_UI_OVERLAY_SYSTEM_H */
