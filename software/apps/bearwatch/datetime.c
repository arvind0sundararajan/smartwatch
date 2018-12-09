/* 
Module for datetime functions, updating time, setting alarms.
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "nrf_delay.h"

#include "display.h"

int datetime_main(void) {
	struct tm *tm;
	time_t curtime = time(0);

	clock_t processor_time = clock();

	for (;;) {
		tm = localtime(&curtime);
		processor_time = clock();

		char line_1_buffer[16];
		sprintf(line_1_buffer, "Time: %02d:%02d:%02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);

		char line_2_buffer[16];
		sprintf(line_2_buffer, "Proc: %d\n", (int) processor_time);

		printf(line_1_buffer);
		printf(line_2_buffer);

		//display_write(line_1_buffer, DISPLAY_LINE_0);
		//display_write(line_2_buffer, DISPLAY_LINE_1);
		nrf_delay_ms(1000);
	}

}
