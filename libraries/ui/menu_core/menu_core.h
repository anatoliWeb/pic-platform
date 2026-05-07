#ifndef LIBRARIES_UI_MENU_CORE_H
#define LIBRARIES_UI_MENU_CORE_H

#include "core/compiler.h"
#include "core/types.h"

typedef void (*menu_action_cb_t)(void* ctx);

typedef enum
{
    MENU_ITEM_ACTION = 0,
    MENU_ITEM_SUBMENU,
    MENU_ITEM_TOGGLE,
    MENU_ITEM_VALUE
} menu_item_type_t;

typedef struct menu_item_s menu_item_t;

struct menu_item_s
{
    const char* label;
    menu_item_type_t type;
    uint8_t flags;

    menu_item_t* parent;
    menu_item_t* first_child;
    menu_item_t* next;
    menu_item_t* prev;

    menu_action_cb_t action;
    void* action_ctx;

    uint8_t toggle_value;
    int16_t value;
    int16_t min_value;
    int16_t max_value;
    int16_t step;
};

typedef struct
{
    menu_item_t* root;
    menu_item_t* current;
    menu_item_t* selected;
} menu_t;

void menu_init(menu_t* menu);
void menu_set_root(menu_t* menu, menu_item_t* root);
menu_item_t* menu_get_current(menu_t* menu);

void menu_next(menu_t* menu);
void menu_prev(menu_t* menu);
void menu_enter(menu_t* menu);
void menu_back(menu_t* menu);

void menu_select(menu_t* menu, menu_item_t* item);
void menu_reset(menu_t* menu);

#endif /* LIBRARIES_UI_MENU_CORE_H */
