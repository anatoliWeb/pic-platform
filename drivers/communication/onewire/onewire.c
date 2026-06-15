#include "drivers/communication/onewire/onewire.h"
/* drivers/communication/onewire/onewire.c */
#if defined(DRV_COMPILER_C18)
    #include "../../../C18/drivers/communication/onewire/onewire.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/drivers/communication/onewire/onewire.c"
#else
    #error "OneWire driver implementation is not available for this compiler."
#endif