#ifndef __TICKER_H
#define __TICKER_H

#define ROSLOG_DEBUG 10
#define ROSLOG_INFO 20
#define ROSLOG_WARN 30
#define ROSLOG_ERROR 40
#define ROSLOG_FATAL 50

class Ticker
{
public:
  Ticker();
  static bool setup(const char *topic_name = "tick");

};

#endif // __TICKER_H
