#include <stdint.h>
#include "stm32f103.h"
#include "pwm.h"

int main(void) {
    pwm_tim2_init();

    while (1) {
        // TIM2 phần cứng tự động phát xung PWM độc lập
    }

    return 0;
}
