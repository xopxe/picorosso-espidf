#ifndef __PICOROSSO_H
#define __PICOROSSO_H

#include "picorosso_config.h"

#include "picoros.h"
#include "picoserdes.h"

#include "rosout.h"

/*
Publishes using a provided buffer for serialization. Buffer
should not shared between threads.
*/
#define pr_publish_buf(publisher, msg, buf, buf_size)        \
  ({                                                         \
    size_t len_ = ps_serialize(buf, &msg, buf_size);         \
    if (len_ > 0)                                            \
    {                                                        \
      picoros_publish(&publisher, buf, len_);                \
    }                                                        \
    else                                                     \
    {                                                        \
      ESP_LOGE("picorosso", "Message serialization error."); \
    }                                                        \
  })

/*
Publishes using an internal buffer of size PUBLISHER_BUF_SIZE
for serialization. Thread safe. Good for easy, sporadic publishing.
*/
#define pr_publish(publisher, msg)                                      \
  ({                                                                    \
    xSemaphoreTake(PicoRosso::bufSemaphore, portMAX_DELAY);             \
    size_t len_ = ps_serialize(PicoRosso::publisher_buf,                \
                               &msg, sizeof(PicoRosso::publisher_buf)); \
    if (len_ > 0)                                                       \
    {                                                                   \
      picoros_publish(&publisher, PicoRosso::publisher_buf, len_);      \
    }                                                                   \
    else                                                                \
    {                                                                   \
      ESP_LOGE("picorosso", "Message serialization error.");            \
    }                                                                   \
    xSemaphoreGive(PicoRosso::bufSemaphore);                            \
  })

class PicoRosso
{
public:
  static bool setup(const char *node_name,
                    const char *zenoh_router_address,
                    const uint32_t domain_id = 0);
  static picoros_node_t node;
  static Rosout rosout;
  static void set_timestamp(ros_Time &stamp);
  static void set_timestamp(ros_Time &stamp, z_clock_t &now);

  // nothing bellow here is for users to use
  static uint8_t publisher_buf[PUBLISHER_BUF_SIZE]; // pre-allocated buffer for serialization
  static SemaphoreHandle_t bufSemaphore;

  PicoRosso();
  PicoRosso(PicoRosso const &);      // Don't Implement
  void operator=(PicoRosso const &); // Don't implement
};

#endif // __PICOROSSO_H
