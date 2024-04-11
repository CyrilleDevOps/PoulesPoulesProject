#include "soc/soc_caps.h"

#define TAG_SLEEP  "Low_Power"

static RTC_DATA_ATTR struct timeval sleep_enter_time;

#define WAKEUP_TIME 3000
#define uS_TO_S_FACTOR 1000000LL

void deep_sleep_task(void *args);
void setup_rtc_timer_wakeup(int wakeup_time_sec );