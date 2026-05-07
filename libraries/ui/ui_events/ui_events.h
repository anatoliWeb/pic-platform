#ifndef LIBRARIES_UI_UI_EVENTS_H
#define LIBRARIES_UI_UI_EVENTS_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/ui/menu_core/menu_core.h"
#include "libraries/ui/menu_navigation/menu_navigation.h"
#include "libraries/ui/menu_widgets/menu_widgets.h"
#include "libraries/ui/input_adapter/input_adapter.h"

#define UI_EVENTS_QUEUE_SIZE 16u
#define UI_EVENTS_MAX_SUBSCRIBERS 8u

typedef enum
{
    UI_EVENT_NONE = 0,
    UI_EVENT_INPUT,
    UI_EVENT_MENU_CHANGED,
    UI_EVENT_MENU_SELECTED,
    UI_EVENT_WIDGET_CHANGED,
    UI_EVENT_SCREEN_CHANGED,
    UI_EVENT_POPUP_OPEN,
    UI_EVENT_POPUP_CLOSE,
    UI_EVENT_NOTIFICATION
} ui_event_type_t;

typedef struct
{
    ui_event_type_t type;
    uint8_t source;
    uint8_t target;
    int16_t param1;
    int16_t param2;
    void* user_data;
} ui_event_t;

typedef void (*ui_event_cb_t)(const ui_event_t* event);

typedef struct
{
    ui_event_t queue[UI_EVENTS_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;

    ui_event_cb_t subscribers[UI_EVENTS_MAX_SUBSCRIBERS];
    uint8_t subscriber_count;
} ui_events_t;

void ui_events_init(ui_events_t* events);
void ui_events_push(ui_events_t* events, const ui_event_t* event);
uint8_t ui_events_pop(ui_events_t* events, ui_event_t* out_event);
uint8_t ui_events_peek(const ui_events_t* events, ui_event_t* out_event);
void ui_events_clear(ui_events_t* events);

uint8_t ui_events_subscribe(ui_events_t* events, ui_event_cb_t cb);
void ui_events_dispatch(ui_events_t* events, const ui_event_t* event);

#endif /* LIBRARIES_UI_UI_EVENTS_H */
