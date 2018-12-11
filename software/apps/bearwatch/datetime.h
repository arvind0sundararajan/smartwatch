#ifndef DATETIME__
#define DATETIME__

#include <time.h>

time_t *initial_time; //time_t object of current time (seconds past epoch)
struct tm *current_time_info; //datetime information struct

// set an alarm in the future
void set_alarm(uint8_t hour, uint8_t minute);

/* increments the time by 1 s.*/
void update_time(void);

int datetime_main(void);

#endif