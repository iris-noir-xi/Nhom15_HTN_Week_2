#include <stdint.h>
#include "stm32f103.h"
#include "gpio.h"
#include "systick.h"

int main(void) {
    led_gpio_init();
    systick_init();

    uint32_t t_pa0 = 0; // 0.1Hz -> chu kỳ 10000ms (đảo trạng thái mỗi 5000ms)
    uint32_t t_pa1 = 0; // 1Hz   -> chu kỳ 1000ms  (đảo trạng thái mỗi 500ms)
    uint32_t t_pa2 = 0; // 10Hz  -> chu kỳ 100ms   (đảo trạng thái mỗi 50ms)

    while (1) {
        uint32_t now = get_millis();

        if (now - t_pa2 >= 50) {
            t_pa2 = now;
            led_toggle(2); // LED 10Hz trên PA2
        }

        if (now - t_pa1 >= 500) {
            t_pa1 = now;
            led_toggle(1); // LED 1Hz trên PA1
        }

        if (now - t_pa0 >= 5000) {
            t_pa0 = now;
            led_toggle(0); // LED 0.1Hz trên PA0
        }
    }

    return 0;
}
