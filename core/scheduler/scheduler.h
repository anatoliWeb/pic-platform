#ifndef CORE_SCHEDULER_SCHEDULER_H
#define CORE_SCHEDULER_SCHEDULER_H

#include "core/compiler.h"
#include "core/types.h"

#define MAX_TASKS 10u

typedef void (*task_callback_t)(void);

typedef struct
{
    task_callback_t callback;
    uint32_t interval;
    uint32_t last_run;

    uint8_t enabled;
    uint8_t run_once;

} task_t;

void scheduler_init(void);
void scheduler_add_task(task_t* task);
void scheduler_run(void);

uint8_t timer_expired(uint32_t* last, uint32_t interval);

#endif /* CORE_SCHEDULER_SCHEDULER_H */
