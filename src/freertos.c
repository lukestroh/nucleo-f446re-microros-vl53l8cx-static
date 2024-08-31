/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************

  Additional information for using FreeRTOS tasks
  static StackType_t myTaskStack[STACK_SIZE];
  static StaticTask_t myTaskTCB;

  xTaskCreateStatic(
      myTaskFunction,    // Function that implements the task
      "TaskName",        // Name of the task
      STACK_SIZE,        // Stack size in words
      (void *)NULL,      // Task input parameter
      tskIDLE_PRIORITY,  // Priority of the task
      myTaskStack,       // Stack buffer
      &myTaskTCB         // Task control block
  );



  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "system.h"

// microROS includes
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rcl/error_handling.h>
#include <rclc/executor.h>
#include <uxr/client/transport.h>
#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>
#include <rosidl_runtime_c/primitives_sequence_functions.h>
#include <branch_detection_interfaces/msg/vl53l8cx8x8.h>

// custom microROS includes
#include "freertos.h"
#include "microros_extras.h"
#include "rcl_utils.h"

// VL53L8CX includes
#include "vl53l8cx_api.h"

typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;

extern UART_HandleTypeDef huart2;

/* Definitions for blinkLED */
osThreadId_t blinkLEDHandle;
uint32_t blinkLEDBuffer[ 128 ];
osStaticThreadDef_t blinkLEDControlBlock;
const osThreadAttr_t blinkLED_attributes = {
  .name = "blinkLED",
  .cb_mem = &blinkLEDControlBlock,
  .cb_size = sizeof(blinkLEDControlBlock),
  .stack_mem = &blinkLEDBuffer[0],
  .stack_size = sizeof(blinkLEDBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for microROS */
osThreadId_t microROSHandle;
uint32_t microROSBuffer[ 3000 ];
osStaticThreadDef_t microROSControlBlock;
const osThreadAttr_t microROS_attributes = {
  .name = "microROS",
  .cb_mem = &microROSControlBlock,
  .cb_size = sizeof(microROSControlBlock),
  .stack_mem = &microROSBuffer[0],
  .stack_size = sizeof(microROSBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for VL53L8CX */
osThreadId_t VL53L8CXHandle;
uint32_t VL53L8CXBuffer[ 8192 ];
osStaticThreadDef_t VL53L8CXControlBlock;
const osThreadAttr_t VL53L8CX_attributes = {
  .name = "VL53L8CX",
  .cb_mem = &VL53L8CXControlBlock,
  .cb_size = sizeof(VL53L8CXControlBlock),
  .stack_mem = &VL53L8CXBuffer[0],
  .stack_size = sizeof(VL53L8CXBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for vl53l8cxMsgMutex */
osMutexId_t vl53l8cxMsgMutexHandle;
osStaticMutexDef_t vl53l8cxMsgMutexControlBlock;
const osMutexAttr_t vl53l8cxMsgMutex_attributes = {
  .name = "vl53l8cxMsgMutex",
  .cb_mem = &vl53l8cxMsgMutexControlBlock,
  .cb_size = sizeof(vl53l8cxMsgMutexControlBlock),
};

/* Create message */
branch_detection_interfaces__msg__Vl53l8cx8x8 msg;


void MX_FREERTOS_Init() {
  /* VL53L8CX mutex */
  vl53l8cxMsgMutexHandle = osMutexNew(&vl53l8cxMsgMutex_attributes);

  /* creation of blink_LED */
  blinkLEDHandle = osThreadNew(StartBlinkLEDTask, NULL, &blinkLED_attributes);

  /* creation of microROS */
  microROSHandle = osThreadNew(StartmicroROSTask, &huart2, &microROS_attributes);

  /* creation of VL53L8CX */
  VL53L8CXHandle = osThreadNew(StartVL53L8CXRanging, NULL, &VL53L8CX_attributes);
  
  // if (VL53L8CXHandle == NULL || blinkLEDHandle == NULL || microROSHandle == NULL) {
  //   Error_Handler();
  // }

}

/**
  * @brief  Function implementing the blink_LED thread.
  * @retval None
  */
void StartBlinkLEDTask(void *argument) {
  /* USER CODE BEGIN 5 */
  uint32_t last_time = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    if (xTaskGetTickCount() - last_time > 1000) {
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      last_time = xTaskGetTickCount();
    }
  }
  /* USER CODE END 5 */
}


/**
* @brief Function implementing the microROS thread.
* @param argument: p_huart2
* @retval None
*/
void StartmicroROSTask(void* argument) {
  /* USER CODE BEGIN StartmicroROSTask */

  // sys_conf_t* sys_conf = (sys_conf_t*)argument;

  rmw_uros_set_custom_transport(
    true,
    (void *) argument,
    cubemx_transport_open,
    cubemx_transport_close,
    cubemx_transport_write,
    cubemx_transport_read
  );

  rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
  freeRTOS_allocator.allocate = microros_allocate;
  freeRTOS_allocator.deallocate = microros_deallocate;
  freeRTOS_allocator.reallocate = microros_reallocate;
  freeRTOS_allocator.zero_allocate =  microros_zero_allocate;

  if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
      // printf("Error on default allocators (line %d)\n", __LINE__);
    Error_Handler();
  }

  // system state
  SystemState system_state = AGENT_WAIT;

  // micro-ROS app
  rcl_publisher_t publisher;
  rclc_support_t support;
  rcl_node_t node;

  rcl_ret_t rcl_status = RCL_RET_OK;
  osStatus_t os_status;

  os_status = osMutexAcquire(vl53l8cxMsgMutexHandle, osWaitForever);
  // Init message
  if (!branch_detection_interfaces__msg__Vl53l8cx8x8__init(&msg)) {
    Error_Handler();
  };
  msg.data.capacity = 64;
  msg.data.size = 64;
  if (!rosidl_runtime_c__int32__Sequence__init(&msg.data, msg.data.size)) {
    Error_Handler();
  }
  os_status = osMutexRelease(vl53l8cxMsgMutexHandle);


  for(;;) {
    switch(system_state) {
      case AGENT_WAIT:
        execute_every_n_ms(500, system_state = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_AVAILABLE : AGENT_WAIT);
        break;
      case AGENT_AVAILABLE:
        system_state = (true == create_rcl_entities(&freeRTOS_allocator, &support, &node, &publisher)) ? AGENT_CONNECTED : AGENT_WAIT;
        if (system_state == AGENT_WAIT) {
          rcl_status = destroy_rcl_entities(&support, &node, &publisher);
        }
        break;
      case AGENT_CONNECTED:
        execute_every_n_ms(10, system_state = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_CONNECTED : AGENT_DISCONNECTED);
        if (system_state == AGENT_CONNECTED) {
          os_status = osMutexAcquire(vl53l8cxMsgMutexHandle, osWaitForever);
          rcl_status = rcl_publish(&publisher, &msg, NULL);
          os_status = osMutexRelease(vl53l8cxMsgMutexHandle);
        }
        break;
      case AGENT_DISCONNECTED:
        rcl_status = destroy_rcl_entities(&support, &node, &publisher);
        system_state = AGENT_WAIT;
        break;
      default:
        break;
    }
  }
  /* USER CODE END StartmicroROSTask */
}

/* USER CODE BEGIN Header_StartVL53L8CXRanging */
/**
* @brief Function implementing the VL53L8CX thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartVL53L8CXRanging */
void StartVL53L8CXRanging(void *argument)
{
  /* USER CODE BEGIN StartVL53L8CXRanging */
  // sys_conf_t* sys_conf = (sys_conf_t*)argument;

  osStatus_t os_status;
  uint8_t sensor_status = VL53L8CX_STATUS_OK;
  uint8_t resolution_, is_alive, data_ready;

  VL53L8CX_Configuration dev_;
  VL53L8CX_ResultsData results_;

  // Set the message data pointer to the results data pointer?
  // os_status = osMutexAcquire(vl53l8cxMsgMutexHandle, osWaitForever);
  // int32_t* p_msg_data = &(msg.data.data);
  // *p_msg_data = results_.distance_mm;
  // os_status = osMutexRelease(vl53l8cxMsgMutexHandle);

  sensor_status = VL53L8CX_Reset_Sensor(&(dev_.platform));

  // // Check if sensor is attached
  while (1) {
    sensor_status = vl53l8cx_is_alive(&dev_, &is_alive);
    if (!is_alive) {
      osDelay(1000);
    } else {
      break;
    }
  }

  // Initialize the sensor. The function copies the firmware (~84 Kbytes) to the module. This is done by loading the code over the I²C/SPI interface, and performing a boot routine to complete the initialization
  sensor_status |= vl53l8cx_init(&dev_);
  sensor_status |= vl53l8cx_set_ranging_mode(&dev_, VL53L8CX_RANGING_MODE_CONTINUOUS);
  sensor_status |= vl53l8cx_set_resolution(&dev_, VL53L8CX_RESOLUTION_8X8);
  sensor_status |= vl53l8cx_set_ranging_frequency_hz(&dev_, 15);
  sensor_status |= vl53l8cx_set_power_mode(&dev_, VL53L8CX_POWER_MODE_WAKEUP);
  sensor_status |= vl53l8cx_start_ranging(&dev_);

  if (sensor_status != VL53L8CX_STATUS_OK) {
    Error_Handler();
  }

  /* Infinite loop */
  for(;;) {
    sensor_status = vl53l8cx_check_data_ready(&dev_, &data_ready);
    if(data_ready){
      sensor_status = vl53l8cx_get_resolution(&dev_, &resolution_);
      sensor_status = vl53l8cx_get_ranging_data(&dev_, &results_);
    

      os_status = osMutexAcquire(vl53l8cxMsgMutexHandle, osWaitForever);
      for (uint32_t i=0; i<64; ++i){
        msg.data.data[i] = results_.distance_mm[i];
      }
      

      os_status = osMutexRelease(vl53l8cxMsgMutexHandle);
    }
  }
  /* USER CODE END StartVL53L8CXRanging */
}



/* USER CODE END Application */
