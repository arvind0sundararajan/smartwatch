#ifndef DATETIME__
#define DATETIME__

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

TimerHandle_t datetime_update_timer_handler;  /**< datetime toggling FreeRTOS timer. */

time_t *initial_time; //time_t object of current time (seconds past epoch)
struct tm *current_time_info; //datetime information struct

/* increments the time by 1 s.*/
void update_time(void);

void datetime_update_timer_callback (void * pvParameter);

int datetime_main(void);

#endif