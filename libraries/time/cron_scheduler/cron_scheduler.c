/*
 * File: libraries/time/cron_scheduler/cron_scheduler.c
 */

#include "libraries/time/cron_scheduler/cron_scheduler.h"

static uint8_t cron_match_field(uint8_t rule_value, uint8_t current_value)
{
    if (rule_value == CRON_ANY)
    {
        return 1u;
    }
    return (uint8_t)(rule_value == current_value ? 1u : 0u);
}

static uint8_t cron_rule_matches(const cron_rule_t* rule, const rtc_datetime_t* dt)
{
    if (cron_match_field(rule->minute, dt->minute) == 0u) { return 0u; }
    if (cron_match_field(rule->hour, dt->hour) == 0u) { return 0u; }
    if (cron_match_field(rule->day, dt->day) == 0u) { return 0u; }
    if (cron_match_field(rule->month, dt->month) == 0u) { return 0u; }
    if (cron_match_field(rule->weekday, dt->weekday) == 0u) { return 0u; }
    return 1u;
}

static uint8_t cron_already_ran_this_minute(const cron_task_t* task, const rtc_datetime_t* dt)
{
    if (task->last_run_year != dt->year) { return 0u; }
    if (task->last_run_month != dt->month) { return 0u; }
    if (task->last_run_day != dt->day) { return 0u; }
    if (task->last_run_hour != dt->hour) { return 0u; }
    if (task->last_run_minute != dt->minute) { return 0u; }
    return 1u;
}

static void cron_mark_ran_now(cron_task_t* task, const rtc_datetime_t* dt)
{
    task->last_run_year = dt->year;
    task->last_run_month = dt->month;
    task->last_run_day = dt->day;
    task->last_run_hour = dt->hour;
    task->last_run_minute = dt->minute;
}

static uint8_t cron_rule_valid(const cron_rule_t* rule)
{
    if (rule == (const cron_rule_t*)0)
    {
        return 0u;
    }

    if ((rule->minute != CRON_ANY) && (rule->minute > 59u)) { return 0u; }
    if ((rule->hour != CRON_ANY) && (rule->hour > 23u)) { return 0u; }
    if ((rule->day != CRON_ANY) && ((rule->day < 1u) || (rule->day > 31u))) { return 0u; }
    if ((rule->month != CRON_ANY) && ((rule->month < 1u) || (rule->month > 12u))) { return 0u; }
    if ((rule->weekday != CRON_ANY) && ((rule->weekday < 1u) || (rule->weekday > 7u))) { return 0u; }

    return 1u;
}

uint8_t cron_scheduler_init(cron_scheduler_t* scheduler, rtc_t* rtc)
{
    uint8_t i;

    if ((scheduler == (cron_scheduler_t*)0) || (rtc == (rtc_t*)0))
    {
        return 0u;
    }

    scheduler->rtc = rtc;

    for (i = 0u; i < CRON_MAX_TASKS; i++)
    {
        scheduler->tasks[i].used = 0u;
        scheduler->tasks[i].enabled = 0u;
        scheduler->tasks[i].callback = (cron_callback_t)0;
        scheduler->tasks[i].callback_ctx = (void*)0;
        scheduler->tasks[i].last_run_year = 0u;
        scheduler->tasks[i].last_run_month = 0u;
        scheduler->tasks[i].last_run_day = 0u;
        scheduler->tasks[i].last_run_hour = 0u;
        scheduler->tasks[i].last_run_minute = 0u;
    }

    scheduler->initialized = 1u;
    return 1u;
}

int8_t cron_scheduler_add(cron_scheduler_t* scheduler,
                          const cron_rule_t* rule,
                          cron_callback_t callback,
                          void* callback_ctx)
{
    uint8_t i;
    cron_task_t* task;

    if ((scheduler == (cron_scheduler_t*)0) || (scheduler->initialized == 0u))
    {
        return -1;
    }

    if ((callback == (cron_callback_t)0) || (cron_rule_valid(rule) == 0u))
    {
        return -1;
    }

    for (i = 0u; i < CRON_MAX_TASKS; i++)
    {
        task = &scheduler->tasks[i];
        if (task->used == 0u)
        {
            task->used = 1u;
            task->enabled = 1u;
            task->rule = *rule;
            task->callback = callback;
            task->callback_ctx = callback_ctx;
            task->last_run_year = 0u;
            task->last_run_month = 0u;
            task->last_run_day = 0u;
            task->last_run_hour = 0u;
            task->last_run_minute = 0u;
            return (int8_t)i;
        }
    }

    return -1;
}

uint8_t cron_scheduler_remove(cron_scheduler_t* scheduler, uint8_t id)
{
    if ((scheduler == (cron_scheduler_t*)0) || (scheduler->initialized == 0u))
    {
        return 0u;
    }

    if (id >= CRON_MAX_TASKS)
    {
        return 0u;
    }

    scheduler->tasks[id].used = 0u;
    scheduler->tasks[id].enabled = 0u;
    scheduler->tasks[id].callback = (cron_callback_t)0;
    scheduler->tasks[id].callback_ctx = (void*)0;
    return 1u;
}

void cron_scheduler_update(cron_scheduler_t* scheduler)
{
    rtc_datetime_t dt;
    uint8_t i;
    cron_task_t* task;

    if ((scheduler == (cron_scheduler_t*)0) || (scheduler->initialized == 0u))
    {
        return;
    }

    if (rtc_get_datetime(scheduler->rtc, &dt) == 0u)
    {
        return;
    }

    for (i = 0u; i < CRON_MAX_TASKS; i++)
    {
        task = &scheduler->tasks[i];

        if ((task->used == 0u) || (task->enabled == 0u) || (task->callback == (cron_callback_t)0))
        {
            continue;
        }

        if (cron_rule_matches(&task->rule, &dt) == 0u)
        {
            continue;
        }

        if (cron_already_ran_this_minute(task, &dt) != 0u)
        {
            continue;
        }

        cron_mark_ran_now(task, &dt);
        task->callback(task->callback_ctx);
    }
}

uint8_t cron_scheduler_enable(cron_scheduler_t* scheduler, uint8_t id)
{
    if ((scheduler == (cron_scheduler_t*)0) || (scheduler->initialized == 0u))
    {
        return 0u;
    }
    if (id >= CRON_MAX_TASKS)
    {
        return 0u;
    }
    if (scheduler->tasks[id].used == 0u)
    {
        return 0u;
    }
    scheduler->tasks[id].enabled = 1u;
    return 1u;
}

uint8_t cron_scheduler_disable(cron_scheduler_t* scheduler, uint8_t id)
{
    if ((scheduler == (cron_scheduler_t*)0) || (scheduler->initialized == 0u))
    {
        return 0u;
    }
    if (id >= CRON_MAX_TASKS)
    {
        return 0u;
    }
    if (scheduler->tasks[id].used == 0u)
    {
        return 0u;
    }
    scheduler->tasks[id].enabled = 0u;
    return 1u;
}
