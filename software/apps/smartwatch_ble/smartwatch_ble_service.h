#ifndef SMARTWATCH_BLE_SERVICE__
#define SMARTWATCH_BLE_SERVICE__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

/* Simple API to create services
   Assumes that all data stored is of size uint8
 */

typedef void (*ble_event_handler)( ble_evt_t const * p_ble_evt, void * p_context);

typedef struct ble_service_params {
	ble_event_handler evt_handler; // Pointer to event handler													 // In implementation, p_context is sent in with app_timer_start. In general, ignore
    ble_uuid128_t uuid;								 // Define a unique value to refer to the service (uuid-gen)
    uint16_t base_service_uuid; 			         // Define a unique value to refer to the service (short)
} ble_service_params;

/* An acceptable uuid is {0x00, 0x01, 0x02, ... }*/

typedef struct smartwatch_ble_service {
    ble_event_handler evt_handler;	// In implementation, p_context is sent in with app_timer_start. In general, ignore
    uint16_t service_handle;    /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    char_handle; /* Handle for the characteristic as provided by BLE stack */ 
    uint8_t  uuid;              // Reference to UUID
    uint16_t base_service_uuid; 
    uint8_t num_characteristics; // number of characteritics associated with the service
    uint16_t conn_handle;
    bool     notification_enabled;
} smartwatch_ble_service;

/* Takes in parameters supplied by the user and returns a more filled out value.
 *
 */
uint32_t smartwatch_ble_service_init(ble_service_params* ble_params, smartwatch_ble_service* ble_service);
uint32_t smartwatch_ble_service_add_char(smartwatch_ble_service* ble_service);
uint32_t smartwatch_ble_service_set_char_value(smartwatch_ble_service* ble_service, uint8_t new_value);

/* COPY PASTA 
   The event handler should have the following form

   void ble_event_handler(ble_evt_t const* p_ble_evt, void* p_context) {
   		ret_code_t err_code = NRF_SUCCESS;

   		switch(p_ble_evt->header.evt_id) {
			case BLE_GAP_EVT_DISCONNECTED:
				break;
			case BLE_GAP_EVT_CONNECTED:
				break;
			case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
				break;
			case BLE_GATTC_EVT_TIMEOUT:
				break;
			case BLE_GATTS_EVT_TIMEOUT:
				break;
			case BLE_GATTS_EVT_WRITE:
				break;
   		}
   }
 */

#endif