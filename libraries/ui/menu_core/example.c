/*
 * File: libraries/ui/menu_core/example.c
 */

#include "core/compiler.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "libraries/ui/menu_core/menu_core.h"

static menu_t g_menu;

static menu_item_t g_root;
static menu_item_t g_settings;
static menu_item_t g_info;
static menu_item_t g_reboot;

static menu_item_t g_toggle_sound;
static menu_item_t g_brightness;

static void action_info(void* ctx)
{
    DRV_UNUSED(ctx);
    DBG_PRINT("Info action\r\n");
}

static void action_reboot(void* ctx)
{
    DRV_UNUSED(ctx);
    DBG_PRINT("Reboot action\r\n");
}

static void link_parent(menu_item_t* parent, menu_item_t* child)
{
    child->parent = parent;
}

static void link_siblings(menu_item_t* left, menu_item_t* right)
{
    left->next = right;
    right->prev = left;
}

static void setup_menu_tree(void)
{
    g_root.label = "Main";
    g_root.type = MENU_ITEM_SUBMENU;
    g_root.parent = (menu_item_t*)0;
    g_root.prev = (menu_item_t*)0;
    g_root.next = (menu_item_t*)0;

    g_settings.label = "Settings";
    g_settings.type = MENU_ITEM_SUBMENU;
    g_settings.action = (menu_action_cb_t)0;
    g_settings.action_ctx = (void*)0;
    g_settings.first_child = &g_toggle_sound;
    g_settings.prev = (menu_item_t*)0;

    g_info.label = "Info";
    g_info.type = MENU_ITEM_ACTION;
    g_info.action = action_info;
    g_info.action_ctx = (void*)0;
    g_info.first_child = (menu_item_t*)0;

    g_reboot.label = "Reboot";
    g_reboot.type = MENU_ITEM_ACTION;
    g_reboot.action = action_reboot;
    g_reboot.action_ctx = (void*)0;
    g_reboot.first_child = (menu_item_t*)0;
    g_reboot.next = (menu_item_t*)0;

    g_root.first_child = &g_settings;

    g_toggle_sound.label = "Sound";
    g_toggle_sound.type = MENU_ITEM_TOGGLE;
    g_toggle_sound.toggle_value = 0u;
    g_toggle_sound.action = (menu_action_cb_t)0;
    g_toggle_sound.first_child = (menu_item_t*)0;
    g_toggle_sound.prev = (menu_item_t*)0;

    g_brightness.label = "Brightness";
    g_brightness.type = MENU_ITEM_VALUE;
    g_brightness.value = 1;
    g_brightness.min_value = 1;
    g_brightness.max_value = 3;
    g_brightness.step = 1;
    g_brightness.action = (menu_action_cb_t)0;
    g_brightness.first_child = (menu_item_t*)0;
    g_brightness.next = (menu_item_t*)0;

    link_parent(&g_root, &g_settings);
    link_parent(&g_root, &g_info);
    link_parent(&g_root, &g_reboot);

    link_siblings(&g_settings, &g_info);
    link_siblings(&g_info, &g_reboot);

    link_parent(&g_settings, &g_toggle_sound);
    link_parent(&g_settings, &g_brightness);
    link_siblings(&g_toggle_sound, &g_brightness);
}

void main(void)
{
    menu_item_t* cur;

    setup_menu_tree();

    menu_init(&g_menu);
    menu_set_root(&g_menu, &g_root);

    /* Navigate root: Settings -> Info -> Reboot. */
    menu_next(&g_menu);
    menu_next(&g_menu);

    /* Action callback demo. */
    menu_enter(&g_menu);

    /* Enter submenu and toggle/value demo. */
    menu_select(&g_menu, &g_settings);
    menu_enter(&g_menu);

    menu_enter(&g_menu);     /* Toggle Sound */
    menu_next(&g_menu);
    menu_enter(&g_menu);     /* Brightness value cycle */

    menu_back(&g_menu);

    cur = menu_get_current(&g_menu);
    if (cur != (menu_item_t*)0)
    {
        DBG_PRINT("Current item: ");
        DBG_PRINT(cur->label);
        DBG_PRINT("\r\n");
    }

    while (1)
    {
        /* Input and rendering are handled in separate modules. */
    }
}
