#ifndef SMARTWATCH_BLE_SERVICE__
#define SMARTWATCH_BLE_SERVICE__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

/* Simple API to create services
   Assumes that all data stored is of size uint8
 */

typedef struct ble_service_params {
    // Pointer to event handler
    // Base Value
    // UUID
}

typedef struct smartwatch_ble_service {
    // Pointer to event handler
    // Base Value
    // UUID
    // number of characteristics
}

/* Takes in parameters supplied by the user and returns a more filled out value.
 *
 */
smartwatch_ble_service* smartwatch_ble_service_init(ble_service_params* ble_parms);
void smartwatch_ble_service_add_char(smartwatch_ble_service* ble_service);

#endif