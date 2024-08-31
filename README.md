# nucleo-f446re-microROS-vl53l8cx

## Adding custom messages to the project
Clone https://github.com/micro-ROS/micro_ros_stm32cubemx_utils into your project. Navigate to `<proj>/micro_ros_stm32cubemx_utils/microros_static_library/library_generation/extra_packages/`.

Recommended: Add your custom message package to extra_packages.repos. Alternatively, packages can be added directly to this directory (not recommended).

## Building `micro_ros_stm32cubemx_utils`

Follow the directions [here](https://github.com/micro-ROS/micro_ros_stm32cubemx_utils).

## Building extra_packages
These steps are outlined in the `micro_ros_stm32cubemx_utils` package, but provided here for ease of use. Run these two commands in the project's root directory.
```
docker pull microros/micro_ros_static_library_builder:humble
docker run -it --rm -v $(pwd):/project --env MICROROS_LIBRARY_FOLDER=micro_ros_stm32cubemx_utils/microros_static_library microros/micro_ros_static_library_builder:humble
```

## Configuring external interrupts on Nucleo-F446RE
```
https://stm32f4-discovery.net/2014/08/stm32f4-external-interrupts-tutorial/
```
