#include "esp_log.h"

#include "picorosso.h"
#include "ticker.h"

static const char *TAG = "ticker";

static picoros_publisher_t publisher_tick = {
    .topic =
        {
            .name = "ticker",
            .type = ROSTYPE_NAME(ros_Int32),
            .rihs_hash = ROSTYPE_HASH(ros_Int32),
        },
};

Ticker::Ticker() {}

static void ticker_task(void *)
{
  TickType_t last_wake_time = xTaskGetTickCount();
  while (true)
  {
    static int loop_count;
    ros_Int32 msg_tick = loop_count++;

    pr_publish(publisher_tick, msg_tick);

    vTaskDelayUntil(&last_wake_time, 1000 / portTICK_PERIOD_MS);
  }
}

bool Ticker::setup(const char *topic_name)
{
  ESP_LOGD(TAG, "Setting up...");

  ESP_LOGI(TAG, "Declaring publisher on [%s]", topic_name);
  publisher_tick.topic.name = topic_name;
  picoros_publisher_declare(&PicoRosso::node, &publisher_tick);

  xTaskCreate(ticker_task, "ticker_task", 4096, NULL, 1, NULL);

  ESP_LOGD(TAG, "Setting up done.");

  return true;
}
