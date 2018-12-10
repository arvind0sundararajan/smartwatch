#ifndef SENSORS__
#define SENSORS__

/* Initialize sensors */
void sensors_init(void);

/* Read temperature */
void read_temperature(float* data);

/* Read humidity */
float read_humidity(void);

/* Read pressure */
float read_pressure(void);

/* Read restart gyro */
void restart_gyro(void);

/* Read update gyro */
void update_gyro_measurement();

/* Read get rotation gyro */
void get_rotation_gyro(float * arr);


#endif
