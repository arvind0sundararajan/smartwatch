#ifndef BLE_PRESSURE_SERVICE__
#define BLE_PRESSURE_SERVICE__

#include "smartwatch_ble_service.h"

smartwatch_ble_service pressure_service;

void pressure_service_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

#endif