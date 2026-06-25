/*
 * File: core/rtos/rtos.c
 */

#include "core/rtos/rtos.h"
#include "core/delay.h"
#include "drivers/timers/tick/tick.h"

/*
 * Internal fallback counter used only when tick driver is not initialized
 * by the application.
 */
static volatile uint32_t g_rtos_fallback_tick_ms = 0u;
static uint8_t g_rtos_enabled = 0u;

void rtos_init(void)
{
#if DRV_USE_FREERTOS
    /*
     * Future hook:
     * FreeRTOS integration will initialize backend objects here.
     */
    g_rtos_enabled = 1u;
#else
    g_rtos_enabled = 0u;
#endif

    g_rtos_fallback_tick_ms = 0u;
}

void rtos_sleep_ms(uint32_t ms)
{
#if DRV_USE_FREERTOS
    /* Future hook: map to vTaskDelay() when RTOS backend is enabled. */
    while (ms != 0u)
    {
        delay_ms(1u);
        rtos_yield_if_needed();
        ms--;
    }
#else
    while (ms != 0u)
    {
        delay_ms(1u);
        g_rtos_fallback_tick_ms++;
        rtos_yield_if_needed();
        ms--;
    }
#endif
}

void rtos_delay_ms(uint32_t ms)
{
    rtos_sleep_ms(ms);
}

void rtos_sleep_us(uint32_t us)
{
    while (us != 0u)
    {
        delay_us(1u);
        us--;
    }
}

uint32_t rtos_get_tick_ms(void)
{
#if DRV_USE_FREERTOS
    /* Future hook: return RTOS tick converted to milliseconds. */
    return g_rtos_fallback_tick_ms;
#else
    /*
     * Preferred source is system tick driver.
     * Fallback counter advances when rtos_delay_ms() is used.
     */
    uint32_t t;

    t = tick_get();
    if (t != 0u)
    {
        return t;
    }

    return g_rtos_fallback_tick_ms;
#endif
}

uint8_t rtos_is_enabled(void)
{
    return g_rtos_enabled;
}

void rtos_yield(void)
{
#if DRV_USE_FREERTOS
    /* Future hook: taskYIELD(); */
#else
    /* Bare-metal cooperative yield hook. */
    DRV_RTOS_YIELD_HOOK();
#endif
}

void rtos_yield_if_needed(void)
{
    rtos_yield();
}

void rtos_enter_critical(void)
{
    DRV_INT_DISABLE();
}

void rtos_exit_critical(void)
{
    DRV_INT_ENABLE();
}

uint8_t rtos_mutex_lock(void* mutex, uint32_t timeout_ms)
{
    (void)mutex;
    (void)timeout_ms;

    /* Placeholder: always succeeds in bare-metal mode. */
    return 1u;
}

void rtos_mutex_unlock(void* mutex)
{
    (void)mutex;
}

uint8_t rtos_queue_send(void* queue, const void* item, uint32_t timeout_ms)
{
    (void)queue;
    (void)item;
    (void)timeout_ms;

    /* Placeholder: queue backend not implemented yet. */
    return 0u;
}

uint8_t rtos_queue_receive(void* queue, void* item, uint32_t timeout_ms)
{
    (void)queue;
    (void)item;
    (void)timeout_ms;

    /* Placeholder: queue backend not implemented yet. */
    return 0u;
}
