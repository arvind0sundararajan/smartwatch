#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_twi_mngr.h"
#include "nrf_power.h"

#include "buckler.h"
#include "si7021.h"
#include "ms5637.h"
#include "mpu9250.h"

/* TODO; move to init function */
NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

/* Gyro Rotation Values */
static float x_rot, y_rot, z_rot;

static void sensor_callback(void * p_context);

/* Initializes TWI and sensor configurations. Call once before
accessing any sensor data  */
void sensors_init(void)
{
  uint32_t error_code = NRF_SUCCESS;
  /* TWI INITIALIZED */
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = BUCKLER_SENSORS_SCL;
  i2c_config.sda = BUCKLER_SENSORS_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_400K;
  error_code = nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  APP_ERROR_CHECK(error_code);

  /* POWER SET */
  printf("dcdcen\n");
  nrf_power_dcdcen_set(1);
  nrf_delay_ms(20);
  printf("init\n");
  si7021_init(&twi_mngr_instance);
  printf("confi\n");
  si7021_config(si7021_mode0);
  printf("msinit\n");
  ms5637_init(&twi_mngr_instance, osr_8192);
  printf("start\n");
  ms5637_start();
  mpu9250_init(&twi_mngr_instance);

}

/* Reads temperature off si7021 in Celsius */
float read_temperature(void)
{

  float si_temperature, humidity;
  si7021_read_temp_and_RH(&si_temperature, &humidity);
  return si_temperature;

}

/* Reads % relative humidity off the si7021 */
float read_humidity(void)
{

  float si_temperature, humidity;
  si7021_read_temp_and_RH(&si_temperature, &humidity);
  return humidity;

}

/* Reads barometric presure off the ms5637 in mBars */
float read_pressure(void)
{

  float ms_temperature, pressure;
  ms5637_get_temperature_and_pressure(&ms_temperature, &pressure);
  return pressure;

}

/* Start gyro measurements */
void restart_gyro(void)
{
  x_rot = 0;
  y_rot = 0;
  z_rot = 0;
}

/* Updates the gyros rotation in degrees. Must call
repeatedly */
void update_gyro_measurement()
{
  mpu9250_measurement_t gyr_measurement = mpu9250_read_gyro();

  float x_rot_amount = gyr_measurement.x_axis/10;
  if (abs(x_rot_amount) > 0.5)
  {
    x_rot += x_rot_amount;
  }
  float y_rot_amount = gyr_measurement.y_axis/10;
  if (abs(y_rot_amount) > 0.5)
  {
    y_rot += y_rot_amount;
  }
  float z_rot_amount = gyr_measurement.z_axis/10;
  if (abs(z_rot_amount) > 0.5)
  {
    z_rot += z_rot_amount;
  }

}

/* Returns rotation of gyro since start in degrees*/
void get_rotation_gyro(float * arr)
{
  arr[0] = x_rot;
  arr[1] = y_rot;
  arr[2] = z_rot;
}

static void sensor_callback(void * p_context) {
  printf("sensor coa");
  // float temp = read_temperature();

  // uint32_t t;
  // memcpy(&t, &temp, sizeof(t));

  // smartwatch_ble_service_set_char_value(&temperature_service, t);
}