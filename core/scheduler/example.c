#include "core/compiler.h"
#include "core/scheduler/scheduler.h"
#include "drivers/tick/tick.h"
#include "drivers/onewire/onewire.h"
#include "drivers/ds18b20/ds18b20.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"

static volatile uint8_t g_led_state = 0u;

static void task_blink_led(void)
{
    g_led_state ^= 1u;
    PORTBbits.RB0 = g_led_state;
}

static void task_read_ds18b20(void)
{
    int16_t temp_x10;

    if (ds18b20_read_temperature_celsius((uint8_t*)0, &temp_x10) != 0u)
    {
        DBG_PRINT("T(C): ");
        DBG_PRINT_INT(temp_x10 / 10);
        DBG_PRINT(".");
        DBG_PRINT_INT((temp_x10 < 0) ? -(temp_x10 % 10) : (temp_x10 % 10));
        DBG_PRINT("\r\n");
    }
    else
    {
        DBG_PRINT("DS18B20 read error\r\n");
    }
}

static void task_debug_print(void)
{
    DBG_PRINT("Scheduler alive\r\n");
}

void main(void)
{
    static task_t led_task;
    static task_t ds_task;
    static task_t dbg_task;

    TRISBbits.TRISB0 = 0u;
    PORTBbits.RB0 = 0u;

    uart_init(9600u);
    onewire_init(&PORTB, &TRISB, 1u);
    tick_init();
    scheduler_init();

    led_task.callback = task_blink_led;
    led_task.interval = 500u;
    led_task.last_run = 0u;

    ds_task.callback = task_read_ds18b20;
    ds_task.interval = 1000u;
    ds_task.last_run = 0u;

    dbg_task.callback = task_debug_print;
    dbg_task.interval = 2000u;
    dbg_task.last_run = 0u;

    scheduler_add_task(&led_task);
    scheduler_add_task(&ds_task);
    scheduler_add_task(&dbg_task);

    while (1)
    {
        scheduler_run();
    }
}
