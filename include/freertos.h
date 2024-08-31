#ifndef __FREERTOS_TASKS_H__
#define __FREERTOS_TASKS_H__

#include "cmsis_os.h"

void MX_FREERTOS_Init();
void StartBlinkLEDTask(void* argument);
void StartmicroROSTask(void* argument);
void StartVL53L8CXRanging(void* argument);

#endif // __FREERTOS_TASKS_H__
