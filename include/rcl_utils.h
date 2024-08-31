#ifndef __RCL_UTILS_H__
#define __RCL_UTILS_H__

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include "system.h"

bool create_rcl_entities(
    rcl_allocator_t* allocator,
    rclc_support_t* support,
    rcl_node_t* node,
    rcl_publisher_t* publisher
);

rcl_ret_t destroy_rcl_entities(
    rclc_support_t* support,
    rcl_node_t* node,
    rcl_publisher_t* publisher
    // rclc_executor_t* executor
);

void execute_every_n_ms(int64_t ms, SystemState system_state);

#endif