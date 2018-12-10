// /* 
// Module for datetime functions, updating time, setting alarms.
// */

// #include <stdbool.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <math.h>
// #include <time.h>

// #include "FreeRTOS.h"
// #include "FreeRTOSConfig.h"
// #include "task.h"
// #include "timers.h"

// #include "nrf_delay.h"

// #include "display.h"

// #define DATETIME_TIMER_PERIOD      1000          /**< Timer period. timer will expire after 1000 ms */

// TimerHandle_t datetime_update_timer_handle;  /**< Reference to time update FreeRTOS timer. */

// time_t *current_time; //time_t object of current time (seconds past epoch)
// struct tm *current_time_info; //datetime information struct

// /* initialize the current_time to num seconds since last epoch.
// Initialize the tm object based on this time.
// */
// static void datetime_init(void) {
// 	time(current_time);
// 	current_time_info = localtime(current_time);
// 	printf("Datetime successfully initialized\n");
// }


// /* increments the time by 1 s.*/
// void update_time(void) {
// 	current_time += 1;
// 	current_time_info = localtime(current_time);
// }


// /* displays the time */
// void display_time(void) {
// 	char line_0_buffer[16];
// 	strftime(line_0_buffer, 16, "%H:%M:%S", current_time_info);
// 	//sprintf(line_0_buffer, "Time: %02d:%02d:%02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
// 	printf(line_0_buffer);
// 	printf("\n");
// 	//display_write(line_0_buffer, DISPLAY_LINE_0);
// }

// /**@brief The function to call when the datetime FreeRTOS timer expires.
//  *
//  * @param[in] pvParameter   Pointer that will be used as the parameter for the timer.
//  */
// static void datetime_update_timer_callback (void * pvParameter)
// {
//     UNUSED_PARAMETER(pvParameter);
//     printf("Callback called\n");
//     update_time();
//     display_time();
// }


// int datetime_main(void) {
// 	datetime_init();

//     /* Start timer for updating time */
//     datetime_update_timer_handle = xTimerCreate( "datetime", DATETIME_TIMER_PERIOD, pdTRUE, NULL, datetime_update_timer_callback);
//     UNUSED_VARIABLE(xTimerStart(datetime_update_timer_handle, 0));
//     return 1;
// }
