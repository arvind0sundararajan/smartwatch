/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** @file
 * @defgroup bearwatch_main main.c
 * @{
 * @ingroup bearwatch
 *
 * @brief FreeRTOS application main file for the bearwatch
 *
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// #include "FreeRTOS.h"
// #include "FreeRTOSConfig.h"
// #include "task.h"
// #include "timers.h"
#include "bsp.h"
#include "nordic_common.h"

#include "sdk_errors.h"
#include "app_error.h"
#include "app_scheduler.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_spi.h"
#include "nrfx_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"

#include "buckler.h"
#include "display.h"

#include "accelerometer.h"
#include "smartwatch_ble_main.h"
#include "sensors.h"

// scheduler settings
#define SCHED_MAX_EVENT_DATA_SIZE 					APP_TIMER_SCHED_EVENT_DATA_SIZE //just a rnadom nubmer for now
#define SCHED_QUEUE_SIZE 							10 // also just a random number for now


// LED array
static uint8_t LEDS[3] = {BUCKLER_LED0, BUCKLER_LED1, BUCKLER_LED2};


/* INIT FUNCTIONS */

/**@brief Function for initializing the nrf log module. (RTT)
 */
static void log_init(ret_code_t error_code)
{
    error_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(error_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
    printf("Log initialized\n");
}

/**@brief Function for initializing power management.
 */
static void power_management_init(ret_code_t error_code)
{
    error_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(error_code);
}

/* Initialize the display.
 */
static void smartwatch_display_init(ret_code_t error_code) {
	// initialize spi master
	nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);
	nrf_drv_spi_config_t spi_config = {
		.sck_pin = BUCKLER_LCD_SCLK,
		.mosi_pin = BUCKLER_LCD_MOSI,
		.miso_pin = BUCKLER_LCD_MISO,
		.ss_pin = BUCKLER_LCD_CS,
		.irq_priority = NRFX_SPI_DEFAULT_CONFIG_IRQ_PRIORITY,
		.orc = 0,
		.frequency = NRF_DRV_SPI_FREQ_4M,
		.mode = NRF_DRV_SPI_MODE_2,
		.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
  	};
	error_code = nrf_drv_spi_init(&spi_instance, &spi_config, NULL, NULL);
	APP_ERROR_CHECK(error_code);

	// initialize display driver
	display_init(&spi_instance);
	printf("Display initialized\n");
	nrf_delay_ms(1000);
}

/* initialize the app_timers module */
static void timers_init(ret_code_t error_code) {
	/* Initialize clock driver for better time accuracy in FREERTOS */
    error_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(error_code);

    /* Request LF clock */
    nrf_drv_clock_lfclk_request(NULL);

    error_code = app_timer_init();
    APP_ERROR_CHECK(error_code);
}


/* Initialize GPIO and LEDs*/
static void gpio_init(ret_code_t error_code) {
    // initialize GPIO driver
    if (!nrfx_gpiote_is_init()) {
        error_code = nrfx_gpiote_init();
    }
    APP_ERROR_CHECK(error_code);
    // configure leds
    // manually-controlled (simple) output, initially set
    nrfx_gpiote_out_config_t out_config = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);
    for (int i=0; i<3; i++) {
        error_code = nrfx_gpiote_out_init(LEDS[i], &out_config);
        APP_ERROR_CHECK(error_code);
    }
    return;
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


int main(void)
{
    ret_code_t err_code = NRF_SUCCESS;

    /* initialize nrf log module */
    log_init(err_code);

    // initialize power management
    power_management_init(err_code);

    /* Initialize the display. */
    smartwatch_display_init(err_code);

    /* initialize app_timers */
    timers_init(err_code);

    // initialize GPIO
    gpio_init(err_code);

    //initialize the nrf scheduler
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);


    display_write("Welcome to", DISPLAY_LINE_0);
    display_write("BearWatch", DISPLAY_LINE_1);


    printf("sensors\n");
    sensors_init();
    printf("sensors done\n");


    smartwatch_ble_main();
    accelerometer_main();

    while (true)
    {
       app_sched_execute();
       idle_state_handle();
    }
}

/**
 *@}
 **/
