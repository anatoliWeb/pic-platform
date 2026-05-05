#include "core/scheduler/scheduler.h"
#include "drivers/tick/tick.h"

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

        if ((now - task->last_run) >= task->interval)
        {
            task->last_run = now;
            task->callback();
        }
    }
}
