/*
 * File: core/scheduler/scheduler.c
 */

#include "core/scheduler/scheduler.h"
#include "core/rtos/rtos.h"

typedef struct
{
    scheduler_task_t* task;
    uint8_t used;
} scheduler_slot_t;

static scheduler_slot_t g_slots[SCHEDULER_MAX_TASKS];

static int16_t scheduler_find_slot(const scheduler_task_t* task)
{
    uint8_t i;

    for (i = 0u; i < SCHEDULER_MAX_TASKS; i++)
    {
        if ((g_slots[i].used != 0u) && (g_slots[i].task == task))
        {
            return (int16_t)i;
        }
    }

    return -1;
}

void scheduler_init(void)
{
    uint8_t i;

    for (i = 0u; i < SCHEDULER_MAX_TASKS; i++)
    {
        g_slots[i].task = (scheduler_task_t*)0;
        g_slots[i].used = 0u;
    }
}

uint8_t scheduler_add_task(scheduler_task_t* task)
{
    uint8_t i;

    if ((task == (scheduler_task_t*)0) || (task->callback == (scheduler_task_cb_t)0) || (task->interval_ms == 0u))
    {
        return 0u;
    }

    if (scheduler_find_slot(task) >= 0)
    {
        return 0u;
    }

    for (i = 0u; i < SCHEDULER_MAX_TASKS; i++)
    {
        if (g_slots[i].used == 0u)
        {
            g_slots[i].task = task;
            g_slots[i].used = 1u;

            task->last_run_ms = rtos_get_tick_ms();
            task->enabled = (uint8_t)(task->enabled != 0u ? 1u : 0u);
            task->run_immediately = (uint8_t)(task->run_immediately != 0u ? 1u : 0u);

            return 1u;
        }
    }

    return 0u;
}

uint8_t scheduler_remove_task(scheduler_task_t* task)
{
    int16_t idx;

    if (task == (scheduler_task_t*)0)
    {
        return 0u;
    }

    idx = scheduler_find_slot(task);
    if (idx < 0)
    {
        return 0u;
    }

    g_slots[(uint8_t)idx].task = (scheduler_task_t*)0;
    g_slots[(uint8_t)idx].used = 0u;

    return 1u;
}

void scheduler_update(uint32_t now_ms)
{
    uint8_t i;

    for (i = 0u; i < SCHEDULER_MAX_TASKS; i++)
    {
        scheduler_task_t* task;

        if (g_slots[i].used == 0u)
        {
            continue;
        }

        task = g_slots[i].task;
        if ((task == (scheduler_task_t*)0) || (task->enabled == 0u))
        {
            continue;
        }

        if (task->run_immediately != 0u)
        {
            task->run_immediately = 0u;
            task->last_run_ms = now_ms;
            task->callback(task->ctx);
            continue;
        }

        if ((uint32_t)(now_ms - task->last_run_ms) >= task->interval_ms)
        {
            task->last_run_ms = now_ms;
            task->callback(task->ctx);
        }
    }
}

uint8_t scheduler_pause_task(scheduler_task_t* task)
{
    if ((task == (scheduler_task_t*)0) || (scheduler_find_slot(task) < 0))
    {
        return 0u;
    }

    task->enabled = 0u;
    return 1u;
}

uint8_t scheduler_resume_task(scheduler_task_t* task)
{
    if ((task == (scheduler_task_t*)0) || (scheduler_find_slot(task) < 0))
    {
        return 0u;
    }

    task->enabled = 1u;
    task->last_run_ms = rtos_get_tick_ms();
    return 1u;
}

uint8_t scheduler_set_interval(scheduler_task_t* task, uint32_t interval_ms)
{
    if ((task == (scheduler_task_t*)0) || (interval_ms == 0u) || (scheduler_find_slot(task) < 0))
    {
        return 0u;
    }

    task->interval_ms = interval_ms;
    return 1u;
}

uint8_t scheduler_run_now(scheduler_task_t* task, uint32_t now_ms)
{
    if ((task == (scheduler_task_t*)0) || (task->callback == (scheduler_task_cb_t)0) || (scheduler_find_slot(task) < 0))
    {
        return 0u;
    }

    task->last_run_ms = now_ms;
    task->callback(task->ctx);
    return 1u;
}
