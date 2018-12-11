#ifndef BLE_RANDOM_DATA_SERVICE__
#define BLE_RANDOM_DATA_SERVICE__

#include "smartwatch_ble_service.h"

smartwatch_ble_service random_data_service;

void random_data_service_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

#endif