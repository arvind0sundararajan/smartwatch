#ifndef DATETIME__
#define DATETIME__

extern time_t *current_time; //time_t object of current time (seconds past epoch)
extern struct tm *current_time_info; //datetime information struct

/* increments the time by 1 s.*/
void update_time(void);

int datetime_main(void);

#endif