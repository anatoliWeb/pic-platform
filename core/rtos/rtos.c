#include "core/rtos/rtos.h"
#include "core/delay.h"

void rtos_delay_ms(uint32_t ms)
{
#if DRV_USE_FREERTOS
    /* TODO: map to vTaskDelay() when FreeRTOS backend is integrated. */
    while (ms != 0u)
    {
        delay_ms(1u);
        ms--;
    }
#else
    while (ms != 0u)
    {
        delay_ms(1u);
        ms--;
    }
#endif
}

uint8_t rtos_mutex_lock(void* mutex, uint32_t timeout_ms)
{
    (void)mutex;
    (void)timeout_ms;

    /* TODO: implement backend-specific lock handling. */
    return 1u;
}

void rtos_mutex_unlock(void* mutex)
{
    (void)mutex;

    /* TODO: implement backend-specific unlock handling. */
}

uint8_t rtos_queue_send(void* queue, const void* item, uint32_t timeout_ms)
{
    (void)queue;
    (void)item;
    (void)timeout_ms;

    /* TODO: implement backend-specific queue send handling. */
    return 0u;
}

uint8_t rtos_queue_receive(void* queue, void* item, uint32_t timeout_ms)
{
    (void)queue;
    (void)item;
    (void)timeout_ms;

    /* TODO: implement backend-specific queue receive handling. */
    return 0u;
}