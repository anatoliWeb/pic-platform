#include "drivers/comparator/comparator.h"

static comparator_mode_t g_cmp_mode = CMP_MODE_OFF;

static uint8_t comparator_mode_bits(comparator_mode_t mode)
{
    switch (mode)
    {
        case CMP_MODE_OFF: return 0x07u;
        case CMP_MODE_1:   return 0x00u;
        case CMP_MODE_2:   return 0x01u;
        case CMP_MODE_3:   return 0x02u;
        default:           return 0x07u;
    }
}

void comparator_init(comparator_mode_t mode)
{
#if defined(CMCON)
    g_cmp_mode = mode;
    CMCON = (uint8_t)((CMCON & 0xF8u) | comparator_mode_bits(mode));
#elif defined(CMCON0)
    g_cmp_mode = mode;
    CMCON0bits.CM = comparator_mode_bits(mode);
#else
    DRV_UNUSED(mode);
#endif
}

void comparator_enable(void)
{
    comparator_init(g_cmp_mode == CMP_MODE_OFF ? CMP_MODE_1 : g_cmp_mode);
}

void comparator_disable(void)
{
#if defined(CMCON)
    CMCON = (uint8_t)((CMCON & 0xF8u) | comparator_mode_bits(CMP_MODE_OFF));
#elif defined(CMCON0)
    CMCON0bits.CM = comparator_mode_bits(CMP_MODE_OFF);
#endif
    g_cmp_mode = CMP_MODE_OFF;
}

uint8_t comparator_get_output(void)
{
#if defined(CMCON)
    return (uint8_t)CMCONbits.COUT;
#elif defined(CMCON0)
    return (uint8_t)CMCON0bits.COUT;
#else
    return 0u;
#endif
}
