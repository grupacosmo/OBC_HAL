#include "run.hpp"

#include <ccl/result.hpp>

using namespace ccl::prelude;

Result<int, int> result_example(bool success) {
    if (success) {
        return Ok { 1 };
    } else {
        return Err { -1 };
    }
}

void run(HardwareHandles handles) {
    result_example(true).unwrap();
    while (true) {
        HAL_GPIO_TogglePin(handles.led_gpio_port, handles.led_pin);
        HAL_Delay(1000);
    }
}
