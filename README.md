# pico-rosso

This is a modular system for pico-ros under espidf and PlatformIO. It allows you to write modules that can interact with ROS2 sending and receiving topics, publishing services, and so on.

For an example of a project that uses this library, see [oruga robot](https://github.com/xopxe/pico-oruga-platformio-espidf).

## Time synchronization

If you do not have NTP available, like when connecting through a serial link, you can use the [Sync-Time service](https://github.com/xopxe/ros2_sync_time_service_ws). 

## Authors and acknowledgment

<jvisca@fing.edu.uy> - [Grupo MINA](https://www.fing.edu.uy/inco/grupos/mina/), Facultad de Ingeniería - Udelar, 2024

## License

Apache 2.0
