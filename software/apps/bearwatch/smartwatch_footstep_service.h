#ifndef BLE_FOOTSTEP_SERVICE__
#define BLE_FOOTSTEP_SERVICE__

#include "smartwatch_ble_service.h"

smartwatch_ble_service footstep_service;

void footstep_service_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

#endif