#include "core/compiler.h"
#include "drivers/communication/spi/spi.h"
#include "libraries/led/rgb_utils/rgb_utils.h"
#include "libraries/led/apa102/apa102.h"

#define APA102_LED_COUNT 16u

static apa102_pixel_t g_pixels[APA102_LED_COUNT];

void main(void)
{
    apa102_t strip;
    uint16_t i;
    uint16_t step;
    hsv_color_t hsv;

    /* SPI setup for APA102 clocked protocol. */
    spi_init_master(SPI_MODE_0, SPI_CLOCK_FOSC_16);

    (void)apa102_init(&strip, g_pixels, APA102_LED_COUNT);

    step = 0u;
    while (1)
    {
        /* Color wipe demo. */
        apa102_clear(&strip);
        for (i = 0u; i < APA102_LED_COUNT; i++)
        {
            if (i <= (uint16_t)(step % APA102_LED_COUNT))
            {
                apa102_set_pixel(&strip, i, rgb_color(255u, 40u, 0u));
            }
        }
        apa102_show(&strip);

        /* Rainbow demo. */
        for (i = 0u; i < APA102_LED_COUNT; i++)
        {
            hsv.h = (uint16_t)((step + (uint16_t)((uint32_t)i * 360u / APA102_LED_COUNT)) % 360u);
            hsv.s = 255u;
            hsv.v = 255u;
            apa102_set_pixel(&strip, i, hsv_to_rgb(hsv));
        }
        apa102_show(&strip);

        /* Brightness demo. */
        apa102_set_brightness(&strip, (uint8_t)((step * 16u) & 0xFFu));
        apa102_show(&strip);

        step++;
    }
}
