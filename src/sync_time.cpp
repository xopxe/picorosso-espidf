#include <sys/time.h>
#include <errno.h>
#include <esp_timer.h>
#include <esp_log.h>
#include "picorosso.h"
#include "sync_time.h"

static const char *TAG = "sync_time";

// Based on NTP: https://en.wikipedia.org/wiki/Network_Time_Protocol
// t0 = time at request sent
// t1 = time at request received (remote)
// t2 = time at response sent (remote)
// t3 = time at response received (local)

static ros_Time t0 = {};
static char msg_token[20]; // 64bit number as string
static int64_t offset_ns;
static int64_t rtt_ns;

SyncTimeStates SyncTime::sync_state = SYNC_TIME_UNSYNCED;

static picoros_publisher_t publisher_sync_request = {
    .topic = {
        .name = NULL, //(char *)"sync_time_request",
        .type = ROSTYPE_NAME(ros_String),
        .rihs_hash = ROSTYPE_HASH(ros_String),
    },
    .opts = {
        .congestion_control = Z_CONGESTION_CONTROL_BLOCK,
        .priority = Z_PRIORITY_REAL_TIME,
        .is_express = true,
    }};

static void sync_response_cb(uint8_t *rx_data, size_t data_len);
picoros_subscriber_t subscription_sync_response = {
    .topic = {
        .name = NULL, //(char *)"sync_time_response",
        .type = ROSTYPE_NAME(ros_TimeReference),
        .rihs_hash = ROSTYPE_HASH(ros_TimeReference),
    },
    .user_callback = sync_response_cb,
};

SyncTime::SyncTime() {}

void SyncTime::trigger_sync()
{
  sync_state = SYNC_TIME_SYNCING;

  // memset(msg_token, 0, sizeof(msg_token));
  sprintf(msg_token, "%llu", esp_timer_get_time()); // a "random" number to a msg_token
  static ros_String msg_sync_time_request = {msg_token};
  // D_printf("sync_time: requsting sync [%s]", msg_token);

  z_clock_t now = z_clock_now();

  pr_publish(publisher_sync_request, msg_sync_time_request);

  t0 = {
      .sec = (int32_t)now.tv_sec,
      .nanosec = (uint32_t)now.tv_nsec,
  };
}

static void sync_response_cb(uint8_t *rx_data, size_t data_len)
{
  z_clock_t now = z_clock_now();
  if (SyncTime::sync_state != SYNC_TIME_SYNCING)
  {
    ESP_LOGW(TAG, "non-requested response detected.");
    return;
  }

  ros_TimeReference msg_sync_response = {};
  if (ps_deserialize(rx_data, &msg_sync_response, data_len))
  {

    // for (int i=0; i<sizeof(msg_token)) {
    //   printf("---> [%d] [%d]\n", token[i] msg_token[i]);
    // }
    if (strcmp(msg_sync_response.source, msg_token) != 0)
    {
      ESP_LOGW(TAG, "errant response detected for [%s]: [%s]", msg_token, msg_sync_response.source);
      return;
    }

    uint64_t t0_ns = (uint64_t)t0.sec * 1000000000 + t0.nanosec;
    uint64_t t1_ns = (uint64_t)msg_sync_response.time_ref.sec * 1000000000 + msg_sync_response.time_ref.nanosec;
    uint64_t t2_ns = (uint64_t)msg_sync_response.header.stamp.sec * 1000000000 + msg_sync_response.header.stamp.nanosec;
    uint64_t t3_ns = (uint64_t)now.tv_sec * 1000000000 + now.tv_nsec;

    offset_ns = ((t1_ns - t0_ns) + (t2_ns - t3_ns)) / 2;
    rtt_ns = (t3_ns - t0_ns) - (t2_ns - t1_ns);

    SyncTime::sync_state = SYNC_TIME_SYNCED;
  }
  else
  {
    ESP_LOGE(TAG, "sync_response message deserialization error");
  }
}

bool SyncTime::synchronize_clock()
{
  ESP_LOGD(TAG, "Waiting for server...");

  uint64_t wait_start_time_us = esp_timer_get_time();
  while (esp_timer_get_time() - wait_start_time_us < WAIT_FOR_SERVER_US && sync_state != SYNC_TIME_SYNCED)
  {
    trigger_sync();
    uint64_t start_time_us = esp_timer_get_time();
    while (esp_timer_get_time() - start_time_us < MESSAGE_TIMEOUT_US && sync_state != SYNC_TIME_SYNCED)
    {
      z_sleep_ms(50);
    }
  }

  if (sync_state != SYNC_TIME_SYNCED)
  {
    ESP_LOGE(TAG, "synch_time responder not found");
    return false;
  }

  ESP_LOGD(TAG, "Synchronizing clock...");
  uint64_t avg_offset_ns = 0;
  uint64_t avg_rtt_ns = 0;
  int hit_count = 0;
  for (int count = 0; count < MESSAGE_COUNT; count++)
  {
    trigger_sync();

    // Wait for a synchronization to complete
    uint64_t start_time_us = esp_timer_get_time();
    while (esp_timer_get_time() - start_time_us < MESSAGE_TIMEOUT_US && sync_state != SYNC_TIME_SYNCED)
    {
      z_sleep_ms(50);
    }
    if (sync_state == SYNC_TIME_SYNCED)
    {
      hit_count++;
      // running average, do not overflow uint64
      avg_offset_ns += offset_ns / hit_count - avg_offset_ns / hit_count;
      avg_rtt_ns += rtt_ns / hit_count - avg_rtt_ns / hit_count;
      ESP_LOGD(TAG, "Reading [%i] offset [%lld] latency [%lld]", count, avg_offset_ns, avg_rtt_ns);
    }
    else
    {
      ESP_LOGW(TAG, "Reading offset [%i] failed", count);
    }
    z_sleep_ms(50);
  }
  ESP_LOGI(TAG, "Clock offset [%lld] rtt [%lld] averaging [%i] values", offset_ns, rtt_ns, hit_count);

  struct timeval tm_vl;
  int sreturn = 0;

  uint64_t corrected_now_us = esp_timer_get_time() + avg_offset_ns / 1000;
  tm_vl.tv_sec = corrected_now_us / 1000000;
  tm_vl.tv_usec = corrected_now_us % 1000000;
  sreturn = settimeofday(&tm_vl, NULL);

  if (sreturn == 0)
  {
    z_clock_t now;
    clock_gettime(CLOCK_REALTIME, &now);
    ESP_LOGD(TAG, "settimeofday executed: %llds, %ldns", now.tv_sec, now.tv_nsec);
    return true;
  }
  else
  {
    ESP_LOGE(TAG, "settimeofday is unsuccessful errno = %d", errno);
    return false;
  }
}

bool SyncTime::setup(const char *topic_request,
                     const char *topic_response,
                     const char *source)
{
  ESP_LOGD(TAG, "Setting up...");

  ESP_LOGI(TAG, "Declaring publisher on [%s]", topic_request);
  publisher_sync_request.topic.name = (char *)topic_request;
  picoros_publisher_declare(&PicoRosso::node, &publisher_sync_request);

  ESP_LOGI(TAG, "Declaring subscriber on [%s]", topic_response);
  subscription_sync_response.topic.name = (char *)topic_response;
  picoros_subscriber_declare(&PicoRosso::node, &subscription_sync_response);

  ESP_LOGD(TAG, "Setting up done.");
  return true;
}
