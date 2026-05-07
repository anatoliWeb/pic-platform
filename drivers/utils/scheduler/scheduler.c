#include "drivers/utils/scheduler/scheduler.h"
#include "drivers/timers/tick/tick.h"

static task_t* g_tasks[MAX_TASKS];
static uint8_t g_task_count = 0u;

void scheduler_init(void)
{
    uint8_t i;

    g_task_count = 0u;
    for (i = 0u; i < MAX_TASKS; i++)
    {
        g_tasks[i] = (task_t*)0;
    }
}

void scheduler_add_task(task_t* task)
{
    if (task == (task_t*)0)
    {
        return;
    }

    if (task->callback == (task_callback_t)0)
    {
        return;
    }

    if (task->interval == 0u)
    {
        return;
    }

    if (g_task_count >= MAX_TASKS)
    {
        return;
    }

    task->last_run = tick_get();
    if (task->enabled > 1u)
    {
        task->enabled = 1u;
    }

    g_tasks[g_task_count] = task;
    g_task_count++;
}

void scheduler_run(void)
{
    uint8_t i;
    uint32_t now = tick_get();

    for (i = 0u; i < g_task_count; i++)
    {
        task_t* task = g_tasks[i];

        if (task == (task_t*)0)
        {
            continue;
        }

        if (task->enabled == 0u)
        {
            continue;
        }

        if ((now - task->last_run) >= task->interval)
        {
            if (task->run_once != 0u)
            {
                task->enabled = 0u;
            }
            else
            {
                task->last_run = now;
            }

            task->callback();
        }
    }
}

uint8_t timer_expired(uint32_t* last, uint32_t interval)
{
    uint32_t now;

    if (last == (uint32_t*)0)
    {
        return 0u;
    }

    now = tick_get();

    if ((now - *last) >= interval)
    {
        *last = now;
        return 1u;
    }

    return 0u;
}
