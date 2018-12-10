/* 
Module for datetime functions, updating time, setting alarms.
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "display.h"

#define DATETIME_TIMER_PERIOD      pdMS_TO_TICKS(1000)          /**< Timer period. timer will expire after 1000 ms */

time_t *initial_time; //time_t object of current time (seconds past epoch)
struct tm *current_time_info; //datetime information struct

TimerHandle_t datetime_update_timer_handler;  /**< datetime toggling FreeRTOS timer. */

/* initialize the current_time to num seconds since last epoch.
Initialize the tm object based on this time.
*/
static void datetime_init(void) {
	time(initial_time);
	current_time_info = localtime(initial_time);
	printf("Datetime successfully initialized\n");
}


/* increments the time by 1 s.*/
void update_time(void) {
	printf("updating\n");
	// current_time_info->tm_sec += 1;
	// mktime(current_time_info);

	// char line_0_buffer[16];
	// strftime(line_0_buffer, 16, "%H:%M:%S", current_time_info);
	// sprintf(line_0_buffer, "Time: %02d:%02d:%02d\n", current_time_info->tm_hour, current_time_info->tm_min, current_time_info->tm_sec);
	//printf(line_0_buffer);
	//display_write(line_0_buffer, DISPLAY_LINE_0);
}


/**@brief The function to call when the datetime  timer expires.
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the timer.
 */
void datetime_update_timer_callback (TimerHandle_t datetime_update_timer_handler)
{
    update_time();
    printf("finished updating\n");
}

/* Initialize the datetime timer. */
static void datetime_timers_init(void) {
	datetime_update_timer_handler = xTimerCreate( "DATETIME", DATETIME_TIMER_PERIOD, pdFALSE, NULL, datetime_update_timer_callback);
    UNUSED_VARIABLE(xTimerStart(datetime_update_timer_handler, 0));
}

int datetime_main(void) {
	datetime_init();
	datetime_timers_init();
	printf("Datetime init complete\n");
	return 1;
}
