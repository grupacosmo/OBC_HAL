#ifndef OBC_RUN_HPP
#define OBC_RUN_HPP

#include "stm32l4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

struct HardwareHandles {
    UART_HandleTypeDef* uart;
    GPIO_TypeDef* led_gpio_port;
    uint16_t led_pin;
};

void run(struct HardwareHandles handles);

#ifdef __cplusplus
}
#endif

#endif