#ifndef __SERIAL_USART_H__
#define __SERIAL_USART_H__

// #include "stm32f4xx_hal_conf.h"
#include "main.h"


/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(UART_HandleTypeDef* huart2);

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
void MX_USB_OTG_FS_PCD_Init(PCD_HandleTypeDef* hpcd_USB_OTG_FS);


#endif // __SERIAL_USART_H__