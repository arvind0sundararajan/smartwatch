#ifndef BLE_SERVICE_MANAGER_H__
#define BLE_SERVICE_MANAGER_H__

#include "nrf_sdh_ble.h"
#include "nordic_common.h"
#include "sdk_common.h"
#include "nrf_gpio.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ble_gatts.h>
#include "ble_srv_common.h"


#include "smartwatch_ble_service.h"

/* Custom services */
#include "smartwatch_timer_service.h"
#include "smartwatch_footstep_service.h"
#include "smartwatch_pressure_service.h"
#include "smartwatch_temp_service.h"
#include "smartwatch_humidity_service.h"

#define NUM_SERVICES NRF_SDH_BLE_VS_UUID_COUNT

smartwatch_ble_service* custom_services[NUM_SERVICES];

void services_init(void);
void manager_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

#endif