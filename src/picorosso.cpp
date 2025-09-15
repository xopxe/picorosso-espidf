#include "picorosso.h"
#include <esp_log.h>
#include <esp_timer.h>

static const char *TAG = "picorosso";
static const char *zenoh_mode = "client";

picoros_node_t PicoRosso::node;
uint8_t PicoRosso::publisher_buf[PUBLISHER_BUF_SIZE];

#if defined(USE_SYNC_TIME)
#include "sync_time.h"
SyncTime sync_time;
#endif

#include "rosout.h"
Rosout rosout;

PicoRosso::PicoRosso() {}

void PicoRosso::set_timestamp(ros_Time &stamp)
{
    z_clock_t now;
    clock_gettime(CLOCK_REALTIME, &now);
    stamp.sec = now.tv_sec;
    stamp.nanosec = now.tv_nsec;
}

void PicoRosso::set_timestamp(ros_Time &stamp, z_clock_t &now)
{
    stamp.sec = now.tv_sec;
    stamp.nanosec = now.tv_nsec;
}

bool PicoRosso::setup(const char *node_name,
                      const char *zenoh_router_address)
{
    PicoRosso::node = {
        .name = (char *)node_name,
        .domain_id = 0,
    };

    // Initialize Pico ROS interface
    picoros_interface_t ifx = {
        .mode = (char *)zenoh_mode,
        .locator = (char *)zenoh_router_address,
    };

    ESP_LOGI(TAG, "Starting pico-ros interface [%s] on [%s]", ifx.mode, ifx.locator);
    while (picoros_interface_init(&ifx) == PICOROS_NOT_READY)
    {
        z_sleep_ms(100);
    }
    ESP_LOGD(TAG, "RMW initialized.");

    ESP_LOGI(TAG, "Starting pico-ros node [%s] domain [%lu]\r", node.name, node.domain_id);
    picoros_node_init(&node);

#if defined(USE_SYNC_TIME)
    // Time synchronization
    sync_time.setup();
    if (!sync_time.synchronize_clock())
    {
        ESP_LOGE(TAG, "synch time failed!");
    }
#endif

    // Initialize auxiliary modules
    rosout.setup("rosout");

    return true;
}
