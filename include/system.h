#ifndef __SYSTEM_H__
#define __SYSTEM_H__


#include <vl53l8cx_msgs/msg/vl53l8cx8x8.h>
#include "main.h"
#include "cmsis_os2.h"

// system configuration struct
typedef struct sys_conf_s {
  vl53l8cx_msgs__msg__Vl53l8cx8x8* p_msg;
  UART_HandleTypeDef* p_huart;
} sys_conf_t;

// microROS system state
typedef enum _SystemState {
    AGENT_WAIT,
    AGENT_AVAILABLE,
    AGENT_CONNECTED,
    AGENT_DISCONNECTED
} SystemState;



#endif // __SYSTEM_H__