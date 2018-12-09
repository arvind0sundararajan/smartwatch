#ifndef BLE_TEMP_SERVICE__
#define BLE_TEMP_SERVICE__

#include "smartwatch_ble_service.h"

smartwatch_ble_service temperature_service;

void temp_service_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

#endif