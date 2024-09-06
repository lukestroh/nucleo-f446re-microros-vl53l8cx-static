#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <stdbool.h>

volatile bool vl53l8cx_data_ready = false; // NOTE: osEvents can be set from ISRs. If not using a Mutex, code structure should include an osEvent in this interrupt

#endif // __INTERRUPTS_H__