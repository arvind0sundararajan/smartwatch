#ifndef BLE_HUMIDITY_SERVICE__
#define BLE_HUMIDITY_SERVICE__

#include "smartwatch_ble_service.h"
extern uint8_t hour;
extern uint8_t minute;
extern uint8_t second;
smartwatch_ble_service time_service;

void time_service_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

#endif
