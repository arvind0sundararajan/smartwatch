#ifndef BLE_TEST_SERVICE__
#define BLE_TEST_SERVICE__

#include "smartwatch_ble_service.h"

smartwatch_ble_service timer_service;

void test_service_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

#endif