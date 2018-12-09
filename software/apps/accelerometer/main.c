// Analog accelerometer app
//
// Reads data from the ADXL327 analog accelerometer

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"
#include "nrfx_gpiote.h"
#include "nrfx_saadc.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "buckler.h"

/* Single shot timer used for interval after footstep */
APP_TIMER_DEF(footstep_timer);
/* Footstep indicator */
bool was_footstep;

// ADC channels
#define X_CHANNEL 0
#define Y_CHANNEL 1
#define Z_CHANNEL 2

// callback for SAADC events
void saadc_callback (nrfx_saadc_evt_t const * p_event) {
  // don't care about adc callbacks
}

// sample a particular analog channel in blocking mode
nrf_saadc_value_t sample_value (uint8_t channel) {
  nrf_saadc_value_t val;
  ret_code_t error_code = nrfx_saadc_sample_convert(channel, &val);
  APP_ERROR_CHECK(error_code);
  return val;
}

static void footstep_handler(){
  was_footstep = false;
}

int main (void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  // initialize analog to digital converter
  nrfx_saadc_config_t saadc_config = NRFX_SAADC_DEFAULT_CONFIG;
  saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;
  error_code = nrfx_saadc_init(&saadc_config, saadc_callback);
  APP_ERROR_CHECK(error_code);

  // initialize analog inputs
  // configure with 0 as input pin for now
  nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(0);
  channel_config.gain = NRF_SAADC_GAIN1_6; // input gain of 1/6 Volts/Volt, multiply incoming signal by (1/6)
  channel_config.reference = NRF_SAADC_REFERENCE_INTERNAL; // 0.6 Volt reference, input after gain can be 0 to 0.6 Volts

  // specify input pin and initialize that ADC channel
  channel_config.pin_p = BUCKLER_ANALOG_ACCEL_X;
  error_code = nrfx_saadc_channel_init(X_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  // specify input pin and initialize that ADC channel
  channel_config.pin_p = BUCKLER_ANALOG_ACCEL_Y;
  error_code = nrfx_saadc_channel_init(Y_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  // specify input pin and initialize that ADC channel
  channel_config.pin_p = BUCKLER_ANALOG_ACCEL_Z;
  error_code = nrfx_saadc_channel_init(Z_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  /*TIMER INTIALIZATION*/
  uint32_t err_code;
  err_code = nrf_drv_clock_init();
  APP_ERROR_CHECK(err_code);
  nrf_drv_clock_lfclk_request(NULL);
  app_timer_init();
  err_code = app_timer_create(&footstep_timer, APP_TIMER_MODE_SINGLE_SHOT, footstep_handler);
  APP_ERROR_CHECK(err_code);



  // initialization complete
  printf("Buckler initialized!\n");

  uint32_t max = 0;
  uint32_t min = UINT32_MAX;
  uint32_t footstep_threshold = 0;
  uint32_t average_value = 0;
  int no_of_footsteps = 0;
  int print_counter = 0;
  int max_min_update_counter = 0;
  uint32_t cum_sum = 0;
  uint32_t avg_sum = 0;
  uint32_t sample_old = 0;
  uint32_t sample_new = 0;
  uint32_t precision = 100;

  /* CALIBRATION */
  // for(int i = 0; i < 200; i++){
  //   for(int j = 0; j < 4; j++){
  //     nrf_saadc_value_t x_val = sample_value(X_CHANNEL);
  //     nrf_saadc_value_t y_val = sample_value(Y_CHANNEL);
  //     nrf_saadc_value_t z_val = sample_value(Z_CHANNEL);
  //     uint32_t cumulative_val = sqrt(pow(x_val,2) + pow(y_val,2) + pow(z_val,2));
  //     cum_sum+=cumulative_val;
  //   }
  //   avg_sum = cum_sum/4;
  //   if(avg_sum > max){
  //     max = avg_sum;
  //   }
  //   if(avg_sum < min){
  //     min = avg_sum;
  //   }
  //   footstep_threshold = (max + min)/2;
  //   cum_sum = 0;
  //   avg_sum = 0;
  // }

  while (1) {
    //Low pass filter, averaging the inputs
    for(int i = 0; i < 4; i++){
      // sample analog inputs
      nrf_saadc_value_t x_val = sample_value(X_CHANNEL);
      nrf_saadc_value_t y_val = sample_value(Y_CHANNEL);
      nrf_saadc_value_t z_val = sample_value(Z_CHANNEL);
      uint32_t cumulative_val = sqrt(pow(x_val,2) + pow(y_val,2) + pow(z_val,2));
      cum_sum += cumulative_val;
    }
    avg_sum = cum_sum/4;
    //if the measurement is more than precision level different, update the new value
    if(avg_sum - sample_old > precision || sample_old - avg_sum < precision){
      //printf("ENTERED PRECISION CHANGE!!!! \n");
      sample_old = sample_new;
      sample_new = avg_sum; //update the sample new to the new value
    }
    /* If the newly registered sample is greater than the footstep threshold and less then old sample,
    i.e, negative slope in acceleration graph */
    if(sample_new > footstep_threshold && sample_new < sample_old && !was_footstep){
      no_of_footsteps++; //increment number of footsteps
      was_footstep = true;
      uint32_t err_code;
      err_code = app_timer_start(footstep_timer, APP_TIMER_TICKS(200), NULL);
      APP_ERROR_CHECK(err_code);
    }

    cum_sum = 0;
    avg_sum = 0;
    sample_old = sample_new; //pass the new samples value to the old register
    print_counter++;
    max_min_update_counter++;
    nrf_delay_ms(10);

    if(print_counter == 10){
      printf("no_of_footsteps: %d\n", no_of_footsteps);
      printf("max: %d\n", max);
      printf("min: %d\n", min);
      printf("sample new: %d\n", sample_new);
      printf("sample old: %d\n", sample_old);
      printf("footstep footstep_threshold: %d\n", footstep_threshold);
      nrf_delay_ms(10);
      print_counter = 0;
    }
    if(max_min_update_counter > 50){
      if(sample_new > max){
        max = sample_new;
        footstep_threshold = (max + min)/2;
      }

      if(sample_new < min){
        min = sample_new;
        footstep_threshold = (max + min)/2;
      }
      max_min_update_counter = 0;
    }
  }
}