#ifndef __MICROROS_EXTRAS_H__
#define __MICROROS_EXTRAS_H__

// #include <rmw_microxrcedds_c/config.h>
#include <uxr/client/transport.h>
#include <stdbool.h>
// #include "FreeRTOS.h"
// #include "task.h"


bool cubemx_transport_open(struct uxrCustomTransport * transport);
bool cubemx_transport_close(struct uxrCustomTransport * transport);
size_t cubemx_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
size_t cubemx_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

void * microros_allocate(size_t size, void * state);
void microros_deallocate(void * pointer, void * state);
void * microros_reallocate(void * pointer, size_t size, void * state);
void * microros_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state);


#endif // __MICROROS_EXTRAS_H__