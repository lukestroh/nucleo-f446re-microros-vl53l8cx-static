#ifndef __VL53L8C_SPI_H__
#define __VL53L8C_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

// #include "stm32f4xx_hal_spi.h"
#include "main.h"
/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
void MX_SPI2_Init(SPI_HandleTypeDef* hspi2);

#ifdef __cplusplus
}
#endif

#endif // __VL53L8C_SPI_H__