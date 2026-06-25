/*
 * File: examples-projects/xc8/ring_buffer.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"
#include "core/ring_buffer/ring_buffer.h"

#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

#define TEST_BUFFER_SIZE  8u

static ring_buffer_t g_rb;
static uint8_t g_storage[TEST_BUFFER_SIZE];

static void print_status(const char* label)
{
    /*
     * Print current ring buffer state.
     *
     * Buffer size is 8, but usable capacity is 7 bytes because
     * one empty slot is used to detect the "full" state.
     */
    DBG_PRINT(label);
    DBG_PRINT(" available=");
    DBG_PRINT_INT((int)rb_available(&g_rb));

    DBG_PRINT(" empty=");
    DBG_PRINT_INT((int)rb_is_empty(&g_rb));

    DBG_PRINT(" full=");
    DBG_PRINT_INT((int)rb_is_full(&g_rb));

    DBG_PRINTLN("");
}

static void push_byte(uint8_t value)
{
    /*
     * Try to push one byte into the ring buffer.
     *
     * rb_push() returns:
     *   1 = byte stored
     *   0 = buffer full or invalid buffer
     */
    DBG_PRINT("PUSH ");
    DBG_PRINT_HEX(value);
    DBG_PRINT(" ");

    if (rb_push(&g_rb, value) != 0u)
    {
        DBG_PRINTLN("OK");
    }
    else
    {
        DBG_PRINTLN("FAIL");
    }

    print_status("  state:");
}

static void pop_byte(void)
{
    uint8_t value = 0u;

    /*
     * Try to pop one byte from the ring buffer.
     *
     * rb_pop() returns:
     *   1 = byte read
     *   0 = buffer empty or invalid buffer
     */
    DBG_PRINT("POP ");

    if (rb_pop(&g_rb, &value) != 0u)
    {
        DBG_PRINT("OK value=");
        DBG_PRINT_HEX(value);
        DBG_PRINTLN("");
    }
    else
    {
        DBG_PRINTLN("FAIL EMPTY");
    }

    print_status("  state:");
}

static void test_basic_push_pop(void)
{
    DBG_PRINTLN("");
    DBG_PRINTLN("=== Test 1: basic push/pop ===");

    push_byte('A');
    push_byte('B');
    push_byte('C');

    pop_byte();
    pop_byte();
    pop_byte();
    pop_byte();
}

static void test_full_buffer(void)
{
    uint8_t i;

    DBG_PRINTLN("");
    DBG_PRINTLN("=== Test 2: fill buffer ===");
    DBG_PRINTLN("Buffer size=8, usable capacity=7");

    /*
     * Push 8 bytes.
     * Expected:
     *   first 7 should be OK
     *   8th should FAIL if RB_OVERWRITE is disabled
     */
    for (i = 0u; i < TEST_BUFFER_SIZE; i++)
    {
        push_byte((uint8_t)('0' + i));
        DRV_DELAY_MS(100);
    }
}

static void test_drain_buffer(void)
{
    DBG_PRINTLN("");
    DBG_PRINTLN("=== Test 3: drain buffer ===");

    /*
     * Pop until the buffer becomes empty.
     */
    while (rb_is_empty(&g_rb) == 0u)
    {
        pop_byte();
        DRV_DELAY_MS(100);
    }

    /*
     * One extra pop shows empty behavior.
     */
    pop_byte();
}

static void test_wrap_around(void)
{
    DBG_PRINTLN("");
    DBG_PRINTLN("=== Test 4: wrap-around ===");

    /*
     * Ring buffer wrap-around test:
     *
     * 1. Push A, B, C, D
     * 2. Pop two bytes
     * 3. Push E, F, G, H
     *
     * This forces head/tail indexes to move around the end of the array.
     */
    push_byte('A');
    push_byte('B');
    push_byte('C');
    push_byte('D');

    pop_byte();
    pop_byte();

    push_byte('E');
    push_byte('F');
    push_byte('G');
    push_byte('H');

    DBG_PRINTLN("");
    DBG_PRINTLN("Reading wrapped data:");

    while (rb_is_empty(&g_rb) == 0u)
    {
        pop_byte();
        DRV_DELAY_MS(100);
    }
}

void main(void)
{
    /*
     * PIC18F452 ring buffer test.
     *
     * This module is software-only.
     * It does not use any external peripheral pins.
     *
     * Proteus needs only UART output:
     *   RC6/TX / pin 25 -> Virtual Terminal RXD
     *   RC7/RX / pin 26 <- Virtual Terminal TXD
     *
     * Virtual Terminal:
     *   9600 baud
     *   8N1
     */

    uart_init(9600u);

    rb_init(&g_rb, g_storage, TEST_BUFFER_SIZE);

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 ring_buffer test");
    DBG_PRINTLN("Software RAM FIFO buffer");
    DBG_PRINTLN("UART: RC6/TX pin25 -> Virtual Terminal RXD");
    DBG_PRINTLN("");

    print_status("Initial state:");

    while (1)
    {
        /*
         * Reinitialize buffer before each full test cycle.
         */
        rb_init(&g_rb, g_storage, TEST_BUFFER_SIZE);

        test_basic_push_pop();

        rb_init(&g_rb, g_storage, TEST_BUFFER_SIZE);

        test_full_buffer();
        test_drain_buffer();

        rb_init(&g_rb, g_storage, TEST_BUFFER_SIZE);

        test_wrap_around();

        DBG_PRINTLN("");
        DBG_PRINTLN("All ring_buffer tests finished.");
        DBG_PRINTLN("Restarting in 3 seconds...");
        DBG_PRINTLN("");

        DRV_DELAY_MS(3000);
    }
}