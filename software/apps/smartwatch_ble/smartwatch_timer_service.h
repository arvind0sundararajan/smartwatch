#ifndef BLE_TIMER_SERVICE__
#define BLE_TIMER_SERVICE__

#include "smartwatch_ble_service.h"

smartwatch_ble_service timer_service;

void timer_service_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

#endif