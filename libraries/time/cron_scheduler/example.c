#include "core/compiler.h"
#include "core/debug.h"
#include "libraries/time/rtc_framework/rtc_framework.h"
#include "libraries/time/software_rtc/software_rtc.h"
#include "libraries/time/cron_scheduler/cron_scheduler.h"

static void task_every_minute(void* ctx)
{
    DRV_UNUSED(ctx);
    DBG_PRINTLN("CRON: wildcard minute task");
}

static void task_daily_noon(void* ctx)
{
    DRV_UNUSED(ctx);
    DBG_PRINTLN("CRON: daily noon task");
}

static void task_weekday_morning(void* ctx)
{
    DRV_UNUSED(ctx);
    DBG_PRINTLN("CRON: weekday morning task");
}

void main(void)
{
    software_rtc_t sw;
    rtc_t rtc;
    cron_scheduler_t cron;
    rtc_datetime_t dt;
    cron_rule_t rule;

    dt.year = 2026u;
    dt.month = 1u;
    dt.day = 5u;
    dt.hour = 11u;
    dt.minute = 58u;
    dt.second = 50u;
    dt.weekday = 1u;

    (void)software_rtc_init(&sw, &dt);
    (void)rtc_init(&rtc, software_rtc_get_driver(), &sw);
    (void)cron_scheduler_init(&cron, &rtc);

    /* Wildcard task: every minute at second 0 (update dedup keeps once/minute). */
    rule.minute = CRON_ANY;
    rule.hour = CRON_ANY;
    rule.day = CRON_ANY;
    rule.month = CRON_ANY;
    rule.weekday = CRON_ANY;
    (void)cron_scheduler_add(&cron, &rule, task_every_minute, (void*)0);

    /* Daily task at 12:00. */
    rule.minute = 0u;
    rule.hour = 12u;
    rule.day = CRON_ANY;
    rule.month = CRON_ANY;
    rule.weekday = CRON_ANY;
    (void)cron_scheduler_add(&cron, &rule, task_daily_noon, (void*)0);

    /* Weekday task at 09:30 every Monday (weekday=1). */
    rule.minute = 30u;
    rule.hour = 9u;
    rule.day = CRON_ANY;
    rule.month = CRON_ANY;
    rule.weekday = 1u;
    (void)cron_scheduler_add(&cron, &rule, task_weekday_morning, (void*)0);

    while (1)
    {
        /* Simulated second tick. */
        software_rtc_tick_seconds(&sw, 1u);

        /* Scheduler polling can be frequent; dedup prevents minute duplicates. */
        cron_scheduler_update(&cron);

        if (rtc_get_datetime(&rtc, &dt) != 0u)
        {
            DBG_PRINT("RTC: ");
            DBG_PRINT_INT((int32_t)dt.hour);
            DBG_PRINT(":");
            DBG_PRINT_INT((int32_t)dt.minute);
            DBG_PRINT(":");
            DBG_PRINT_INT((int32_t)dt.second);
            DBG_PRINTLN("");
        }
    }
}
