#include <cstdint>
#ifndef __ROSOUT_H
#define __ROSOUT_H

#define ROSLOG_DEBUG 10
#define ROSLOG_INFO 20
#define ROSLOG_WARN 30
#define ROSLOG_ERROR 40
#define ROSLOG_FATAL 50

class Rosout
{
public:
  Rosout();
  static bool setup(const char *topic_name = "rosout");

  static void out(const char *s,
                  const char *file = "",
                  const char *func = "",
                  uint32_t line = 0,
                  uint8_t level = ROSLOG_INFO);

};

#endif // __ROSOUT_H
