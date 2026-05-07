#ifndef CORE_SCHEDULER_SCHEDULER_H
#define CORE_SCHEDULER_SCHEDULER_H

#include "core/compiler.h"
#include "core/types.h"

typedef void (*scheduler_task_cb_t)(void* ctx);

#ifndef SCHEDULER_MAX_TASKS
#define SCHEDULER_MAX_TASKS 10u
#endif

typedef struct
{
    scheduler_task_cb_t callback;
    void* ctx;

    uint32_t interval_ms;
    uint32_t last_run_ms;

    uint8_t enabled;
    uint8_t run_immediately;
} scheduler_task_t;

void scheduler_init(void);
uint8_t scheduler_add_task(scheduler_task_t* task);
uint8_t scheduler_remove_task(scheduler_task_t* task);

void scheduler_update(uint32_t now_ms);

uint8_t scheduler_pause_task(scheduler_task_t* task);
uint8_t scheduler_resume_task(scheduler_task_t* task);

uint8_t scheduler_set_interval(scheduler_task_t* task, uint32_t interval_ms);
uint8_t scheduler_run_now(scheduler_task_t* task, uint32_t now_ms);

#endif /* CORE_SCHEDULER_SCHEDULER_H */
