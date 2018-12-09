#include "smartwatch_ble_service_manager.h"

/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void)
{
    /* Timer */
    ble_service_params timer_service_params = {
        .base_service_uuid = 0x1400,
        .evt_handler = timer_service_evt_handler,
        .uuid = {{0xBC, 0x8A, 0xBF, 0x45, 0xCA, 0x05, 0x50, 0xBA, 0x40, 0x42, 0xB0, 0x00, 0xC9, 0xAD, 0x64, 0xF3}}
    };
    smartwatch_ble_service_init(&timer_service_params, &timer_service);
    NRF_SDH_BLE_OBSERVER(timer_service_obs,
        BLE_HRS_BLE_OBSERVER_PRIO,
        timer_service_evt_handler,
        &timer_service
    );

    /* Footstep */
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

    /* Pressure */
    ble_service_params pressure_params = {
        .base_service_uuid = 0x1600,
        .evt_handler = pressure_evt_handler,
        .uuid = {
           { 0xE2, 0xCF, 0xCA, 0xC4, 0xC1, 0xB0, 0xA4, 0x86, 0x73, 0x5A, 0x42, 0x35, 0x35, 0x0F, 0x04, 0x00 }
       }
    };
    smartwatch_ble_service_init(&pressure_service_params, &pressure_service);
    NRF_SDH_BLE_OBSERVER(pressure_service_obs,
        BLE_HRS_BLE_OBSERVER_PRIO,
        pressure_evt_handler,
        &pressure_service
    );

    /* Temp */
    ble_service_params temp_service_params = {
        .base_service_uuid = 0x1600,
        .evt_handler = temp_service_evt_handler,
        .uuid = {
           { 0xE2, 0xCF, 0xCA, 0xC4, 0xC1, 0xB0, 0xA4, 0x86, 0x73, 0x5A, 0x42, 0x35, 0x35, 0x0F, 0x04, 0x00 }
       }
    };
    smartwatch_ble_service_init(&temp_service_params, &temp_service);
    NRF_SDH_BLE_OBSERVER(temp_service_obs,
        BLE_HRS_BLE_OBSERVER_PRIO,
        temp_service_evt_handler,
        &temp_service
    );

    /* Humidity */
    ble_service_params humidity_service_params = {
        .base_service_uuid = 0x1600,
        .evt_handler = humidity_service_evt_handler,
        .uuid = {
           { 0xE2, 0xCF, 0xCA, 0xC4, 0xC1, 0xB0, 0xA4, 0x86, 0x73, 0x5A, 0x42, 0x35, 0x35, 0x0F, 0x04, 0x00 }
       }
    };
    smartwatch_ble_service_init(&humidity_service_params, &humidity_service);
    NRF_SDH_BLE_OBSERVER(humidity_service_obs,
        BLE_HRS_BLE_OBSERVER_PRIO,
        humidity_service_evt_handler,
        &humidity_service
    );

    custom_services[0] = &timer_service;
    custom_services[1] = &footstep_service;
    custom_services[2] = &pressure_service;

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
