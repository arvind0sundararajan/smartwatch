#ifndef BLE_HUMIDITY_SERVICE__
#define BLE_HUMIDITY_SERVICE__

#include "smartwatch_ble_service.h"

smartwatch_ble_service humidity_service;

void humidity_service_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

#endif