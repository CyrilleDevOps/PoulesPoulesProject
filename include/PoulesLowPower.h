#include "soc/soc_caps.h"

static RTC_DATA_ATTR struct timeval sleep_enter_time;


#define WAKEUP_TIME 3600
#define uS_TO_S_FACTOR 1000000LL

void deep_sleep_task(void *args);
void setup_rtc_timer_wakeup(void);