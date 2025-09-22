#include <esp_log.h>

#include "picorosso.h"
#include "rosout.h"
#include "picoros.h"
#include "picoserdes.h"

static const char *TAG = "picorosso";

static picoros_publisher_t publisher_log = {
    .topic =
        {
            .name = "rosout",
            .type = ROSTYPE_NAME(ros_Log),
            .rihs_hash = ROSTYPE_HASH(ros_Log),
        },
};

Rosout::Rosout() {}

bool Rosout::setup(const char *topic_name)
{
    ESP_LOGD(TAG, "Seting up...");

    ESP_LOGI(TAG, "Declaring publisher on [%s]", topic_name);
    publisher_log.topic.name = topic_name;
    picoros_publisher_declare(&PicoRosso::node, &publisher_log);

    ESP_LOGD(TAG, "Setting up done.");

    z_sleep_ms(100);
    Rosout::out("New logger active.", __FILE__, __func__, __LINE__, ROSLOG_INFO);
    return true;
}

void Rosout::out(const char *s,
                 const char *file,
                 const char *func,
                 uint32_t line,
                 uint8_t level)
{
    ros_Log msg_log = {
        .level = level,
        .name = (char *)PicoRosso::node.name,
        .msg = (char *)s,
        .file = (char *)file,
        .function = (char *)func,
        .line = line,
    };
    PicoRosso::set_timestamp(msg_log.stamp);

    pr_publish(publisher_log, msg_log);
}
