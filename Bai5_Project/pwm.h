#ifndef PWM_H
#define PWM_H

#include <stdint.h>

void pwm_tim2_ch1_init(void);
void pwm_set_duty(uint8_t duty_percent);

#endif
