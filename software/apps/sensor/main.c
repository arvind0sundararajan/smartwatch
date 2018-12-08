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

NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);


int main(void) {
  // init softdevice
  //nrf_sdh_enable_request();
  //sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);

ret_code_t error_code = NRF_SUCCESS;

// initialize RTT library
error_code = NRF_LOG_INIT(NULL);
APP_ERROR_CHECK(error_code);
NRF_LOG_DEFAULT_BACKENDS_INIT();
printf("Log initialized\n");

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

  float humidity, temperature = 0;

  while (1) {
    nrf_delay_ms(5000);
    si7021_read_temp_and_RH(&temperature, &humidity);
    printf("humidity: %f, temperature: %f\n", humidity, temperature);
    //__WFE();
  }
}
