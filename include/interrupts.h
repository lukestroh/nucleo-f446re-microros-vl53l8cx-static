#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <stdbool.h>

volatile bool vl53l8cx_data_ready = false; // TODO: Events can be set from ISRs.

#endif // __INTERRUPTS_H__