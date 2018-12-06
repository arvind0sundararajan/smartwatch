#ifndef BLE_SERVICE_MANAGER_H__
#define BLE_SERVICE_MANAGER_H__

#include "nrf_sdh_ble.h"
#include "nordic_common.h"
#include "sdk_common.h"
#include "nrf_gpio.h"


#include "smartwatch_ble_service.h"

/* Custom services */
#include "smartwatch_test_service.h"
#include "smartwatch_test_service_2.h"

smartwatch_ble_service timer_service;
smartwatch_ble_service test_service_2;
smartwatch_ble_service test_service_3;

smartwatch_ble_service* custom_services[3];

void services_init(void);
void manager_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

#endif