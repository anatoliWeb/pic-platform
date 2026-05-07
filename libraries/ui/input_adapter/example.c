#include "core/compiler.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "libraries/ui/input_adapter/input_adapter.h"

static input_adapter_t g_input;

/* Demo-only stubs for illustration of mixed sources.
 * Replace with real initialized modules in integration.
 */
static button_t g_button;
static encoder_t g_encoder;
static keypad_matrix_t g_keypad;

static void print_event(input_event_t ev)
{
    switch (ev)
    {
        case INPUT_EVENT_UP:     DBG_PRINT("UP\r\n"); break;
        case INPUT_EVENT_DOWN:   DBG_PRINT("DOWN\r\n"); break;
        case INPUT_EVENT_LEFT:   DBG_PRINT("LEFT\r\n"); break;
        case INPUT_EVENT_RIGHT:  DBG_PRINT("RIGHT\r\n"); break;
        case INPUT_EVENT_SELECT: DBG_PRINT("SELECT\r\n"); break;
        case INPUT_EVENT_BACK:   DBG_PRINT("BACK\r\n"); break;
        case INPUT_EVENT_HOLD:   DBG_PRINT("HOLD\r\n"); break;
        case INPUT_EVENT_NONE:
        default:
            break;
    }
}

void main(void)
{
    input_event_t ev;

    /*
     * In a real project, initialize button/encoder/keypad first,
     * then pass pointers to input_adapter_init().
     */
    (void)input_adapter_init(&g_input, &g_button, &g_encoder, &g_keypad);

    while (1)
    {
        /* Poll all configured sources and translate to UI events. */
        input_adapter_update(&g_input);

        ev = input_adapter_get_event(&g_input);
        if (ev != INPUT_EVENT_NONE)
        {
            print_event(ev);
        }
    }
}
