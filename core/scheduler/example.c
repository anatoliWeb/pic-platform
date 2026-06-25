/*
 * File: core/scheduler/example.c
 */

#include "core/rtos/rtos.h"
#include "core/scheduler/scheduler.h"

typedef struct
{
    uint8_t led_state;
} app_ctx_t;

static app_ctx_t g_app = { 0u };

static void task_blink(void* ctx)
{
    app_ctx_t* app;

    app = (app_ctx_t*)ctx;
    app->led_state ^= 1u;

    /* Application-specific GPIO toggle can be placed here. */
}

static void task_service(void* ctx)
{
    (void)ctx;

    /* Service polling/update task. */
}

static void task_ui(void* ctx)
{
    (void)ctx;

    /* UI/animation update task. */
}

void scheduler_example_run(void)
{
    scheduler_task_t blink_task;
    scheduler_task_t service_task;
    scheduler_task_t ui_task;

    rtos_init();
    scheduler_init();

    blink_task.callback = task_blink;
    blink_task.ctx = &g_app;
    blink_task.interval_ms = 500u;
    blink_task.last_run_ms = 0u;
    blink_task.enabled = 1u;
    blink_task.run_immediately = 1u;

    service_task.callback = task_service;
    service_task.ctx = (void*)0;
    service_task.interval_ms = 100u;
    service_task.last_run_ms = 0u;
    service_task.enabled = 1u;
    service_task.run_immediately = 0u;

    ui_task.callback = task_ui;
    ui_task.ctx = (void*)0;
    ui_task.interval_ms = 20u;
    ui_task.last_run_ms = 0u;
    ui_task.enabled = 1u;
    ui_task.run_immediately = 1u;

    (void)scheduler_add_task(&blink_task);
    (void)scheduler_add_task(&service_task);
    (void)scheduler_add_task(&ui_task);

    while (1)
    {
        uint32_t now;

        now = rtos_get_tick_ms();
        scheduler_update(now);

        /* Optional low-cost cooperative sleep. */
        rtos_delay_ms(1u);
    }
}
