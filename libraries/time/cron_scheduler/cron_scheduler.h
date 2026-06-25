/*
 * File: libraries/time/cron_scheduler/cron_scheduler.h
 */

#ifndef LIBRARIES_TIME_CRON_SCHEDULER_H
#define LIBRARIES_TIME_CRON_SCHEDULER_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/time/rtc_framework/rtc_framework.h"

#ifndef CRON_MAX_TASKS
#define CRON_MAX_TASKS 8u
#endif

#define CRON_ANY 0xFFu

typedef void (*cron_callback_t)(void* ctx);

typedef struct
{
    uint8_t minute;   /* 0..59 or CRON_ANY */
    uint8_t hour;     /* 0..23 or CRON_ANY */
    uint8_t day;      /* 1..31 or CRON_ANY */
    uint8_t month;    /* 1..12 or CRON_ANY */
    uint8_t weekday;  /* 1..7 or CRON_ANY */
} cron_rule_t;

typedef struct
{
    uint8_t used;
    uint8_t enabled;

    cron_rule_t rule;
    cron_callback_t callback;
    void* callback_ctx;

    uint16_t last_run_year;
    uint8_t last_run_month;
    uint8_t last_run_day;
    uint8_t last_run_hour;
    uint8_t last_run_minute;
} cron_task_t;

typedef struct
{
    rtc_t* rtc;
    cron_task_t tasks[CRON_MAX_TASKS];
    uint8_t initialized;
} cron_scheduler_t;

uint8_t cron_scheduler_init(cron_scheduler_t* scheduler, rtc_t* rtc);
int8_t cron_scheduler_add(cron_scheduler_t* scheduler,
                          const cron_rule_t* rule,
                          cron_callback_t callback,
                          void* callback_ctx);
uint8_t cron_scheduler_remove(cron_scheduler_t* scheduler, uint8_t id);
void cron_scheduler_update(cron_scheduler_t* scheduler);

uint8_t cron_scheduler_enable(cron_scheduler_t* scheduler, uint8_t id);
uint8_t cron_scheduler_disable(cron_scheduler_t* scheduler, uint8_t id);

#endif /* LIBRARIES_TIME_CRON_SCHEDULER_H */
