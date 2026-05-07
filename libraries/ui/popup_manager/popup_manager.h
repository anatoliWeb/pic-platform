#ifndef LIBRARIES_UI_POPUP_MANAGER_H
#define LIBRARIES_UI_POPUP_MANAGER_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/ui/screen_manager/screen_manager.h"
#include "libraries/ui/ui_events/ui_events.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/ui/ui_themes/ui_themes.h"
#include "libraries/ui/icon_pack/icon_pack.h"

#define POPUP_MAX_BUTTONS 3u
#define POPUP_QUEUE_SIZE 4u

typedef enum
{
    POPUP_TYPE_INFO = 0,
    POPUP_TYPE_WARNING,
    POPUP_TYPE_ERROR,
    POPUP_TYPE_CONFIRM,
    POPUP_TYPE_NOTIFICATION
} popup_type_t;

typedef enum
{
    POPUP_RESULT_NONE = 0,
    POPUP_RESULT_OK,
    POPUP_RESULT_CANCEL,
    POPUP_RESULT_YES,
    POPUP_RESULT_NO,
    POPUP_RESULT_TIMEOUT
} popup_result_t;

typedef struct
{
    const char* label;
    popup_result_t result;
} popup_button_t;

typedef struct
{
    popup_type_t type;
    const char* title;
    const char* message;
    ui_icon_id_t icon;

    popup_button_t buttons[POPUP_MAX_BUTTONS];
    uint8_t button_count;
    uint8_t selected_button;

    popup_result_t result;
    uint32_t timeout_ms;
    uint32_t started_ms;

    void* user_data;
} popup_t;

typedef struct
{
    popup_t active;
    uint8_t active_valid;

    popup_t queue[POPUP_QUEUE_SIZE];
    uint8_t q_head;
    uint8_t q_tail;

    graphics_t* graphics;
    const ui_theme_t* theme;
    ui_events_t* events;
    screen_manager_t* screens;

    uint8_t dim_background;
} popup_manager_t;

uint8_t popup_manager_init(popup_manager_t* mgr,
                           graphics_t* graphics,
                           const ui_theme_t* theme,
                           ui_events_t* events,
                           screen_manager_t* screens);

uint8_t popup_show(popup_manager_t* mgr, const popup_t* popup);
void popup_close(popup_manager_t* mgr, popup_result_t result);
void popup_update(popup_manager_t* mgr, uint32_t now_ms);
void popup_draw(popup_manager_t* mgr);

uint8_t popup_is_active(const popup_manager_t* mgr);
popup_result_t popup_get_result(const popup_manager_t* mgr);

#endif /* LIBRARIES_UI_POPUP_MANAGER_H */
