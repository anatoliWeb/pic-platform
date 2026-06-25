/*
 * File: libraries/ui/menu_navigation/menu_navigation.h
 */

#ifndef LIBRARIES_UI_MENU_NAVIGATION_H
#define LIBRARIES_UI_MENU_NAVIGATION_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/ui/menu_core/menu_core.h"
#include "libraries/ui/input_adapter/input_adapter.h"

typedef struct
{
    menu_t* menu;
    input_adapter_t* input;
    uint8_t enabled;
} menu_navigation_t;

uint8_t menu_navigation_init(menu_navigation_t* nav, menu_t* menu, input_adapter_t* input);
void menu_navigation_update(menu_navigation_t* nav);

void menu_navigation_set_enabled(menu_navigation_t* nav, uint8_t enabled);
void menu_navigation_reset(menu_navigation_t* nav);

#endif /* LIBRARIES_UI_MENU_NAVIGATION_H */
