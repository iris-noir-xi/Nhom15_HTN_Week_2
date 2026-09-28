#include "systick.h"
#include "stm32f103.h"

volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) {
    ms_ticks++;
}

void systick_init(void) {
    // Clock HSI mặc định 8MHz -> 1ms cần 8000 tick
    SYST_RVR = 8000 - 1;
    SYST_CVR = 0;
    // Bật SysTick: CLKSOURCE = 1 (CPU core clock), TICKINT = 1, ENABLE = 1
    SYST_CSR = (1 << 2) | (1 << 1) | (1 << 0);
}

uint32_t get_millis(void) {
    return ms_ticks;
}
