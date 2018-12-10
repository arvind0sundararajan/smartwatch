/* 
Module for datetime functions, updating time, setting alarms.
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "app_error.h"
#include "app_timer.h"

#include "nrf_delay.h"

#include "display.h"

#define DATETIME_TIMER_PERIOD      APP_TIMER_TICKS(1000)          /**< Timer period. timer will expire after 1000 ms */

time_t *initial_time; //time_t object of current time (seconds past epoch)
struct tm *current_time_info; //datetime information struct

APP_TIMER_DEF(m_datetime_id);

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
	current_time_info->tm_sec += 1;
	mktime(current_time_info);

	char line_0_buffer[16];
	strftime(line_0_buffer, 16, "%H:%M:%S", current_time_info);
	sprintf(line_0_buffer, "%02d:%02d:%02d\n", current_time_info->tm_hour, current_time_info->tm_min, current_time_info->tm_sec);
	display_write(line_0_buffer, DISPLAY_LINE_0);
}


/**@brief The function to call when the datetime FreeRTOS timer expires.
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the timer.
 */
static void datetime_update_timer_handler (void * p_context)
{
    UNUSED_PARAMETER(p_context);
    update_time();
}

/* Initialize the datetime timer. */
static void datetime_timers_init(void) {
	ret_code_t err_code;

	err_code = app_timer_create(&m_datetime_id, APP_TIMER_MODE_REPEATED, datetime_update_timer_handler);
	APP_ERROR_CHECK(err_code);
}

/* start the timers */
static void datetime_timers_start(void) {
	ret_code_t err_code;
	err_code = app_timer_start(m_datetime_id, DATETIME_TIMER_PERIOD, NULL);
	APP_ERROR_CHECK(err_code); 
}

int datetime_main(void) {
	datetime_init();
	datetime_timers_init();
	datetime_timers_start();
	return 1;
}

