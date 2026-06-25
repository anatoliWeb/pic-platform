/*
 * File: libraries/ui/menu_navigation/example.c
 */

#include "core/compiler.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "libraries/ui/input_adapter/input_adapter.h"
#include "libraries/ui/menu_core/menu_core.h"
#include "libraries/ui/menu_navigation/menu_navigation.h"

static menu_t g_menu;
static menu_navigation_t g_nav;
static input_adapter_t g_input;

/* Demo-only input source objects. Initialize with real drivers in app. */
static button_t g_button;
static encoder_t g_encoder;
static keypad_matrix_t g_keypad;

static menu_item_t g_root;
static menu_item_t g_settings;
static menu_item_t g_info;
static menu_item_t g_toggle_sound;
static menu_item_t g_brightness;

static void action_info(void* ctx)
{
    DRV_UNUSED(ctx);
    DBG_PRINT("Info action\r\n");
}

static void setup_tree(void)
{
    g_root.label = "Main";
    g_root.type = MENU_ITEM_SUBMENU;
    g_root.parent = (menu_item_t*)0;
    g_root.first_child = &g_settings;

    g_settings.label = "Settings";
    g_settings.type = MENU_ITEM_SUBMENU;
    g_settings.parent = &g_root;
    g_settings.first_child = &g_toggle_sound;
    g_settings.prev = (menu_item_t*)0;
    g_settings.next = &g_info;

    g_info.label = "Info";
    g_info.type = MENU_ITEM_ACTION;
    g_info.parent = &g_root;
    g_info.first_child = (menu_item_t*)0;
    g_info.action = action_info;
    g_info.prev = &g_settings;
    g_info.next = (menu_item_t*)0;

    g_toggle_sound.label = "Sound";
    g_toggle_sound.type = MENU_ITEM_TOGGLE;
    g_toggle_sound.parent = &g_settings;
    g_toggle_sound.toggle_value = 0u;
    g_toggle_sound.prev = (menu_item_t*)0;
    g_toggle_sound.next = &g_brightness;

    g_brightness.label = "Brightness";
    g_brightness.type = MENU_ITEM_VALUE;
    g_brightness.parent = &g_settings;
    g_brightness.value = 1;
    g_brightness.min_value = 1;
    g_brightness.max_value = 5;
    g_brightness.step = 1;
    g_brightness.prev = &g_toggle_sound;
    g_brightness.next = (menu_item_t*)0;
}

void main(void)
{
    menu_item_t* cur;

    setup_tree();

    menu_init(&g_menu);
    menu_set_root(&g_menu, &g_root);

    (void)input_adapter_init(&g_input, &g_button, &g_encoder, &g_keypad);
    (void)menu_navigation_init(&g_nav, &g_menu, &g_input);

    while (1)
    {
        /* Poll input and update menu state. */
        menu_navigation_update(&g_nav);

        cur = menu_get_current(&g_menu);
        if (cur != (menu_item_t*)0)
        {
            DBG_PRINT("Selected: ");
            DBG_PRINT(cur->label);
            DBG_PRINT("\r\n");
        }
    }
}
