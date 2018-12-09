#include "smartwatch_footstep_service.h"
// #include "sdk_common.h"
// #include "ble_cus.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ble_gatts.h>
#include "ble_srv_common.h"
// #include "nrf_gpio.h"
// #include "boards.h"
#include "nrf_log.h"

void footstep_service_evt_handler(ble_evt_t const * p_ble_evt, void * p_context) {
	// ret_code_t err_code = NRF_SUCCESS;

	smartwatch_ble_service* service = (smartwatch_ble_service*) p_context;
	ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

	if (service != &footstep_service) {
		return;
	}

	switch(p_ble_evt->header.evt_id) {
		case BLE_GAP_EVT_DISCONNECTED:
			break;
		case BLE_GAP_EVT_CONNECTED:
			NRF_LOG_INFO("footstep service  connected");
			break;
		case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
			break;
		case BLE_GATTC_EVT_TIMEOUT:
			break;
		case BLE_GATTS_EVT_TIMEOUT:
			break;
		case BLE_GATTS_EVT_WRITE:
			if (p_evt_write->handle == service->char_handle.value_handle) {
				printf("Characteristic written to. Value: 0x%X (%d)\n", *p_evt_write->data, *p_evt_write->data);

			}
			break;
	}
}
