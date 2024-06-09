#ifndef _SYSTEM_TIMER_H_
#define _SYSTEM_TIMER_H_

#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif

/* TIMER.C */
// void timer_wait(int ticks);
void timer_install();
int64_t get_tick_count();
void wait(uint32_t ms);
const char *asciitime();
typedef int os_time_t;
typedef int os_suseconds_t;
struct os_timeval
{
  os_time_t tv_sec;       /* seconds */
  os_suseconds_t tv_usec; /* microseconds */
};
struct os_timezone
{
  int tz_minuteswest; /* minutes west of Greenwich */
  int tz_dsttime;     /* type of DST correction */
};

/* RTC.C */
void read_rtc();
#ifdef __cplusplus
}
#endif
#endif

