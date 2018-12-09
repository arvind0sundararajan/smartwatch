#include "smartwatch_ble_service.h"
#include "sdk_common.h"
#include <string.h>
#include "ble_srv_common.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_log.h"

uint32_t smartwatch_ble_service_init(ble_service_params* ble_params, smartwatch_ble_service* service) {
	if (ble_params == NULL) {
		return NRF_ERROR_NULL;
	}
	service->evt_handler = ble_params->evt_handler;
	// NRF_LOG_INFO("initialize stuff2.");
	service->service_handle = 0; // To be set later
	// NRF_LOG_INFO("initialize stuff3.");
	service->base_service_uuid = ble_params->base_service_uuid;
	// NRF_LOG_INFO("initialize stuff4.");

	service->conn_handle = BLE_CONN_HANDLE_INVALID;

	uint32_t err_code;
	ble_uuid_t ble_uuid;

	ble_uuid128_t base_uuid = ble_params->uuid;
	err_code = sd_ble_uuid_vs_add(&base_uuid, &service->uuid);

	if (err_code == NRF_ERROR_INVALID_ADDR) {
		NRF_LOG_INFO("NRF_ERROR_INVALID_ADDR");
	}
	if (err_code == NRF_ERROR_FORBIDDEN) {
		NRF_LOG_INFO("NRF_ERROR)NRF_ERROR_FORBIDDEN");
	}
	if (err_code == NRF_ERROR_NO_MEM) {
		NRF_LOG_INFO("NRF_ERROR_NO_MEM");
	}

	if (err_code != NRF_SUCCESS) {
		NRF_LOG_INFO("error code %d", err_code);
		return NRF_ERROR_NULL;
	}

	ble_uuid.type = service->uuid;
	ble_uuid.uuid = service->base_service_uuid;

	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &service->service_handle);

	if (err_code != NRF_SUCCESS) {
		NRF_LOG_INFO("err_code")
		return NRF_ERROR_NULL;
	}

	err_code = smartwatch_ble_service_add_char(service);

	NRF_LOG_INFO("characteristic added.");
	if (err_code != NRF_SUCCESS) {
		return NRF_ERROR_NULL;
	}

	/* As of this point, service has several things that have not been set
	   2. Notification Timer which has not yet been set
	   3. Timer id which should be set
	 */

	return NRF_SUCCESS;
}

uint32_t smartwatch_ble_service_add_char(smartwatch_ble_service* ble_service) {
	if (ble_service == NULL) {
		return NRF_ERROR_NULL;
	}

	/* As of this point, service has several things that have not been set
	   1. char_handle
	   2. Notification Timer which has not yet been set
	   3. Timer id which should be set
	   4.
	 */

	uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // Add Custom Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

	// I assume that the base_service_uuid is set such that the user
	// has properly separated the uuids
    ble_uuid.type = ble_service->uuid;
    ble_uuid.uuid = ble_service->base_service_uuid + 1;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = cccd_md.read_perm;
    attr_md.write_perm = cccd_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint32_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint32_t);

    err_code = sd_ble_gatts_characteristic_add(ble_service->service_handle, &char_md,
                                               &attr_char_value,
                                               &ble_service->char_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

uint32_t smartwatch_ble_service_set_char_value(smartwatch_ble_service* ble_service, uint32_t new_value) {
	if (ble_service == NULL) {
		return NRF_ERROR_NULL;
	}

	uint32_t err_code = NRF_SUCCESS;
	ble_gatts_value_t gatts_value;

	memset(&gatts_value, 0, sizeof(gatts_value));
	gatts_value.len    = sizeof(uint32_t);
	gatts_value.offset = 0;
	gatts_value.p_value= (uint8_t*)&new_value;

	err_code = sd_ble_gatts_value_set(ble_service->conn_handle, ble_service->char_handle.value_handle, &gatts_value);

	// if (err_code == NRF_ERROR_INVALID_ADDR) {
	// 	NRF_LOG_INFO("NRF_ERROR_INVALID_ADDR");
	// }
	// if (err_code == NRF_ERROR_INVALID_PARAM) {
	// 	NRF_LOG_INFO("NRF_ERROR_INVALID_PARAM");
	// }
	// if (err_code == NRF_ERROR_NOT_FOUND) {
	// 	NRF_LOG_INFO("NRF_ERROR_NOT_FOUND");2
	// }
	// if (err_code == NRF_ERROR_DATA_SIZE) {
	// 	NRF_LOG_INFO("NRF_ERROR_DATA_SIZE");
	// }
	// if (err_code == BLE_ERROR_INVALID_CONN_HANDLE) {
	// 	NRF_LOG_INFO("BLE_ERROR_INVALID_CONN_HANDLE");
	// }
	// if (err_code == BLE_ERROR_GATTS_INVALID_ATTR_TYPE) {
	// 	NRF_LOG_INFO("BLE_ERROR_GATTS_INVALID_ATTR_TYPE");
	// }
	if (err_code != NRF_SUCCESS) {
		return err_code;
	}



	/* If Notifying */
	if (ble_service->conn_handle != BLE_CONN_HANDLE_INVALID) {
		ble_gatts_hvx_params_t hvx_params;
		memset(&hvx_params, 0, sizeof(hvx_params));
		hvx_params.handle = ble_service->char_handle.value_handle;
		hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
		hvx_params.offset = gatts_value.offset;
		hvx_params.p_len = &gatts_value.len;
		hvx_params.p_data= gatts_value.p_value;

		sd_ble_gatts_hvx(ble_service->conn_handle, &hvx_params); // TODO: ERR_CODE?
		NRF_LOG_INFO("sd_ble_gatts_hvx result: %x. \r\n", err_code);

	}

	return err_code;
}
