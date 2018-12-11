#ifndef BLE_DATETIME_SERVICE__
#define BLE_DATETIME_SERVICE__

#include "smartwatch_ble_service.h"
#include "datetime.h"

smartwatch_ble_service datetime_service;
void datetime_service_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

#endif
