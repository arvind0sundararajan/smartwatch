#ifndef DATETIME__
#define DATETIME__

time_t *initial_time; //time_t object of current time (seconds past epoch)
struct tm *current_time_info; //datetime information struct

/* increments the time by 1 s.*/
void update_time(void);

int datetime_main(void);

#endif