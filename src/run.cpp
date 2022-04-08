#include "run.hpp"

void run(HardwareHandles handles) {
    while (true) {
        HAL_GPIO_TogglePin(handles.led_gpio_port, handles.led_pin);
        HAL_Delay(1000);
    }
}
