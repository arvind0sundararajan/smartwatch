#include "smartwatch_ble_service_manager.h"

/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void)
{
    ble_service_params timer_service_params = {
        .base_service_uuid = 0x1400,
        .evt_handler = test_service_evt_handler,
        .uuid = {{0xBC, 0x8A, 0xBF, 0x45, 0xCA, 0x05, 0x50, 0xBA, 0x40, 0x42, 0xB0, 0x00, 0xC9, 0xAD, 0x64, 0xF3}}
    };
    smartwatch_ble_service_init(&timer_service_params, &timer_service);
    NRF_SDH_BLE_OBSERVER(test_service_obs,
        BLE_HRS_BLE_OBSERVER_PRIO,
        test_service_evt_handler,
        &timer_service
    );

    ble_service_params footstep_service_params = {
        .base_service_uuid = 0x1500,
        .evt_handler = footstep_service_evt_handler,
        .uuid = {
            {0x20, 0x73, 0x03, 0xc7, 0x3d, 0x90, 0xfa, 0x9c, 0x40, 0x45, 0xf6, 0xea, 0x8e, 0x2e, 0x85, 0xc4}
            }
    };
    smartwatch_ble_service_init(&footstep_service_params, &footstep_service);
    NRF_SDH_BLE_OBSERVER(footstep_service_obs,
        BLE_HRS_BLE_OBSERVER_PRIO,
        footstep_service_evt_handler,
        &footstep_service
    );

    ble_service_params test_service_3_params = {
        .base_service_uuid = 0x1600,
        .evt_handler = test_service_2_evt_handler,
        .uuid = {
           {0x8a, 0xa8, 0x63, 0x16, 0x77, 0x19,0xdd,0xb8, 0xae, 0x44,0x50,0x71, 0x7d,0x77,0x6d,0x6a }
       }
    };
    smartwatch_ble_service_init(&test_service_3_params, &test_service_3);
    NRF_SDH_BLE_OBSERVER(test_service_3_obs,
        BLE_HRS_BLE_OBSERVER_PRIO,
        test_service_2_evt_handler,
        &test_service_3
    );

    custom_services[0] = &timer_service;
    custom_services[1] = &footstep_service;
    // custom_services[2] = &test_service_3;

    NRF_SDH_BLE_OBSERVER(manager_obs,
        BLE_HRS_BLE_OBSERVER_PRIO,
        manager_evt_handler, 
        NULL
    );
}

void manager_evt_handler(ble_evt_t const * p_ble_evt, void * p_context) {
    switch(p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_DISCONNECTED:
            for (int i = 0; i < 3; i ++) {
                smartwatch_ble_service* service = custom_services[i];
                service->conn_handle = BLE_CONN_HANDLE_INVALID;
            }
            break;
        case BLE_GAP_EVT_CONNECTED:
            for (int i = 0; i < 3; i ++) {
                smartwatch_ble_service* service = custom_services[i];
                service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            }
            printf("connected test service 2\n");
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
