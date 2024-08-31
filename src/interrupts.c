#include "interrupts.h"
#include "main.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == INT_C_Pin) {
        vl53l8cx_data_ready = true;
    }
}
