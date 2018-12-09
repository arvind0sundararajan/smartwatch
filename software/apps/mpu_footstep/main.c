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
#include "nrf_drv_gpiote.h"

#include "mpu9250.h"
#include "buckler.h"


NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);
static const nrf_twi_mngr_t* i2c_manager = NULL;
static uint8_t MPU_ADDRESS = 0x68;
static uint8_t MAG_ADDRESS = 0x0C;

// #define PEDSTD_STEPCTR (54 * 16)
static uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr) {
  uint8_t rx_buf = 0;
  nrf_twi_mngr_transfer_t const read_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 1, 0),
  };
  ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);
  APP_ERROR_CHECK(error_code);
  return rx_buf;
}

static void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {
  uint8_t buf[2] = {reg_addr, data};
  nrf_twi_mngr_transfer_t const write_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, buf, 2, 0),
  };
  ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
}


void start_motion_detection_low_power(void){

  uint32_t val, error_code;
  // Make sure accelerometer is running
  val = 0x09;
  i2c_reg_write(MPU_ADDRESS, MPU9250_PWR_MGMT_1, val);

  // Enable accelerometer, disable gyro
  val = 0x07;
  i2c_reg_write(MPU_ADDRESS, MPU9250_PWR_MGMT_2, val);
  // Set Accel LPF setting to 184 Hz Bandwidth
  val = 0x05;
  i2c_reg_write(MPU_ADDRESS, MPU9250_ACCEL_CONFIG, val);
  val = 0x40;
  // Enable Motion Interrupt
  i2c_reg_write(MPU_ADDRESS, MPU9250_INT_ENABLE, 0x40);
  // Enable Accel Hardware Intelligence
  //In MOT_DETECT_CTRL (0x69), set ACCEL_INTEL_EN = 1 and ACCEL_INTEL_MODE = 1
  i2c_reg_write(MPU_ADDRESS, MPU9250_MOT_DETECT_CTRL, 0xC0);
  // Set Motion Threshold
  //In WOM_THR (0x1F), set the WOM_Threshold[7:0] to 1~255 LSBs (0~1020mg)
  i2c_reg_write(MPU_ADDRESS, MPU9250_WOM_THR, 0xFF);
  // Set Frequency of Wake-up
  //In LP_ACCEL_ODR (0x1E), set Lposc_clksel[3:0] = 0.24Hz ~ 500Hz
  i2c_reg_write(MPU_ADDRESS, MPU9250_LP_ACCEL_ODR, 0x03);

  // Enable Cycle Mode (Accel Low Power Mode)
  //In PWR_MGMT_1 (0x6B) make CYCLE =1
  i2c_reg_write(MPU_ADDRESS, MPU9250_PWR_MGMT_1, 0x20);

  printf("%s: MPU6500_StartMotionDetection\r\n", print_timestamp());
}

static void gpio_init(void) {
  ret_code_t err_code;
  err_code = nrf_drv_gpiote_init();
  printf("%s: nrf_drv_gpiote_init_err_code: %lu\r\n", print_timestamp(), err_code);
  APP_ERROR_CHECK(err_code);

  nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

  err_code = nrf_drv_gpiote_in_init(MPU9250_INT_PIN_CFG, &in_config, int_pin_handler);
  printf("%s: nrf_drv_gpiote_in_init_err_code: %lu\r\n", print_timestamp(), err_code);
  APP_ERROR_CHECK(err_code);
  nrf_drv_gpiote_in_event_enable(MPU9250_INT_PIN_CFG, true);
}

void int_pin_handler(void){
  printf("Entered this boi! \n");
}

// static uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr) {
//   uint8_t rx_buf = 0;
//   nrf_twi_mngr_transfer_t const read_transfer[] = {
//     NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
//     NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 1, 0),
//   };
//   ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);
//   APP_ERROR_CHECK(error_code);
//   return rx_buf;
// }
//
// static void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {
//   uint8_t buf[2] = {reg_addr, data};
//   nrf_twi_mngr_transfer_t const write_transfer[] = {
//     NRF_TWI_MNGR_WRITE(i2c_addr, buf, 2, 0),
//   };
//   ret_code_t error_code = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
//   APP_ERROR_CHECK(error_code);
// }
//
//
// void read_pedometer(void){
//   printf("Entering this bitch! \n");
//   // read values
//   int8_t x_val = i2c_reg_read(MPU_ADDRESS, MPU9250_GYRO_PED_SH);
//   // int16_t x_val = (((uint32_t)i2c_reg_read(MPU_ADDRESS, MPU9250_GYRO_PED_SH)) << 24) | (((uint32_t)i2c_reg_read(MPU_ADDRESS, MPU9250_GYRO_PED_H)) << 16) | (((uint32_t)i2c_reg_read(MPU_ADDRESS, MPU9250_GYRO_PED_LH)) << 8) | i2c_reg_read(MPU_ADDRESS, MPU9250_GYRO_PED_L);
//
//   printf("step count: %x", x_val);
//   // convert to g
//   // coversion at +/- 2 g is 16384 LSB/g
//   // mpu9250_measurement_t measurement = {0};
//   // measurement.x_axis = ((float)x_val) / 16384;
//   // measurement.y_axis = ((float)y_val) / 16384;
//   // measurement.z_axis = ((float)z_val) / 16384;
//   // return measurement;
// }
int main(void){
  ret_code_t error_code = NRF_SUCCESS;

  gpio_init();
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
  mpu9250_init(&twi_mngr_instance);
  printf("MPU-9250 initialized\n");
  start_motion_detection_low_power();

}
