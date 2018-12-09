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


int sensors_main(void) {
  // init softdevice
  //nrf_sdh_enable_request();
  //sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);

  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized\n");

  /* TODO: initialize in function */
  // initialize i2c master (two wire interface)
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = BUCKLER_SENSORS_SCL;
  i2c_config.sda = BUCKLER_SENSORS_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_400K;
  error_code = nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  APP_ERROR_CHECK(error_code);

  //Set DC to DC converter
  nrf_power_dcdcen_set(1);

  printf("\nTEMP/HUMIDITY TEST\n");

  printf("twi initialized! \n");


  nrf_delay_ms(20);
  si7021_init(&twi_mngr_instance);
  si7021_config(si7021_mode0);
  ms5637_init(&twi_mngr_instance, osr_8192);
  ms5637_start();
  // initialize MPU-9250 driver
  mpu9250_init(&twi_mngr_instance);
  printf("MPU-9250 initialized\n");

  // loop forever
  float x_rot = 0;
  float y_rot = 0;
  float z_rot = 0;
  float pressure, si_temperature;
  float humidity, ms_temperature = 0;

  while (1) {
    nrf_delay_ms(5000);
    si7021_read_temp_and_RH(&si_temperature, &humidity);
    printf("Relative humidity: %f, si temperature in celsius: %f\n", humidity, si_temperature);
    nrf_delay_ms(1000);
    ms5637_get_temperature_and_pressure(&ms_temperature, &pressure);
    // temperature = ms5637_get_temperature();
    printf("Pressure in mbar: %f, ms temperature in celsius: %f \n, ",pressure, ms_temperature);
    mpu9250_measurement_t gyr_measurement = mpu9250_read_gyro();
    //__WFE();

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

    printf("                      X-Axis\t    Y-Axis\t    Z-Axis\n");
    printf("                  ----------\t----------\t----------\n");
    printf("Angle  (degrees): %10.3f\t%10.3f\t%10.3f\n", x_rot, y_rot, z_rot);
    nrf_delay_ms(100);
  }
}
