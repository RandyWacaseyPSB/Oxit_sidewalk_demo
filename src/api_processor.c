/**
 * @file api_processor.c
 * @author Ankit Bansal (ankit.bansal@oxit.com)
 * @brief
 * @version 0.1
 * @date 2024-04-15
 *
 * @copyright Copyright (c) 2024
 *
 * @copyright Copyright (c) 2024
 * Confidentiality and Proprietary Rights Statement
 * The sample, Code and Hardware, provided at no cost to the customer,
 * contains confidential and proprietary information belonging exclusively
 * to Oxit LLC. All contents, including but not limited to concepts, ideas,
 * designs, methodologies, processes, technologies, and intellectual property,
 * are the sole property of Oxit LLC and are provided for evaluation purposes
 * only.
 *
 * Oxit LLC does not grant any intellectual property rights or permit any
 * other usage of the sample hardware and code beyond evaluation.
 *
 * Unauthorized use, disclosure, distribution, copying, or any form of
 * dissemination of the information contained in this sample is strictly
 * prohibited and may result in legal action.
 *
 * The recipient of this sample agrees to maintain the information's
 * confidentiality and use it only for the purposes explicitly permitted under
 * this agreement.
 *
 * Any exceptions to the proprietary rights and ownership as stated herein must
 * be explicitly acknowledged and agreed upon in writing by Oxit LLC.
 * Failure to comply with these terms may result in immediate termination of any
 * agreements and potential legal consequences.
 *
 * By accessing this sample, you acknowledge and agree to these terms:
 *
 * 1. Limited Use: You may use this Code and Hardware solely to evaluate the
 *    hardware specified by Oxit, LLC in a non-production environment.
 *    Any other use is strictly prohibited.
 *
 * 2. No Rights Granted: This Code and Hardware does not convey any rights,
 *    licenses, or permissions beyond limited evaluation use. Oxit, LLC
 *    retains all intellectual property rights in the Code and Hardware.
 *
 * 3. No Commercial Use: You do not have any rights to use this Code and
 *    Hardware for commercial purposes, incorporate it into any product or
 *    service, or otherwise exploit it commercially.
 *
 * 4. No Distribution: You may not distribute, share, sublicense, or transfer
 *    this Code and Hardware to any third parties without express written
 *    consent from Oxit, LLC.
 *
 * 5. Confidentiality: You agree to keep this Code and Hardware confidential
 *    and not disclose it to unauthorized parties.
 *
 * 6. No Warranty: This Code and Hardware is provided "AS IS" without any
 *    warranties, express or implied.
 *
 * 7. Termination: Your right to use this Code and Hardware terminates
 *    automatically if you breach any of these terms or upon request from
 *    Oxit, LLC.
 *
 * If you do not agree to these terms, you must immediately cease any use of
 * this Code and Hardware and return all copies to Oxit, LLC.
 */

/******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <stdint.h>
#include "commands_defs.h"
#include "frame_parse.h"
#include "api_processor.h"
#include <string.h>

/******************************************************************************
 * EXTERN VARIABLES
 ******************************************************************************/

/******************************************************************************
 * PRIVATE MACROS AND DEFINES
 ******************************************************************************/

/******************************************************************************
 * PRIVATE TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 * STATIC VARIABLES
 ******************************************************************************/

/******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

/******************************************************************************
 * STATIC FUNCTION PROTOTYPES
 ******************************************************************************/
static api_processor_status_t api_processor_parse_response_frame(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len,api_processor_response_t *p_response);
static api_processor_status_t api_processor_parse_get_event_reset(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response);
static api_processor_status_t api_processor_parse_get_event(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len,api_processor_response_t *p_response);
static api_processor_status_t api_processor_get_event_tx_status(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response);
static api_processor_status_t api_processor_get_event_down_data(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response);
static api_processor_status_t api_processor_parse_lorawan_down_data(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response);
static api_processor_status_t api_processor_parse_sid_down_data(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response);
static api_processor_status_t api_processor_parse_get_version(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response);
static api_processor_status_t api_processor_parse_cmd_with_len_zero(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response,const char* module);
static api_processor_status_t api_processor_parse_eui_cmd(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response,const char* eui_type);
static api_processor_status_t api_processor_parse_get_class_cmd(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response,const char* eui_type);
static api_processor_status_t api_procesor_parse_lorawan_class_switch(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response);
static api_processor_status_t api_processor_parse_get_event_seg(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response);
static api_processor_status_t api_processor_parse_get_file_status(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response);
static api_processor_status_t api_processor_handle_request_uplink(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response);

/******************************************************************************
 * STATIC FUNCTIONS
 ******************************************************************************/

/**
 * @brief This function parses the response frame and fills up the
 *        api_processor_response_t structure.
 *
 * @param[in] mcm_module Pointer to the MCM module structure.
 * @param[in] data Pointer to the data buffer containing the response frame.
 * @param[in] len Length of the data buffer containing the response frame.
 * @param[out] p_response Pointer to the api_processor_response_t structure
 *                       that needs to be filled up.
 *
 * @return API_PROCESSOR_SUCCESS if the response is parsed successfully,
 *         otherwise appropriate error code.
 */
static api_processor_status_t api_processor_parse_response_frame(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len,api_processor_response_t *p_response)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    p_response->return_code = data[0];
    p_response->cmd_type = data[1];

    // parse the command and its data according to the command type
    p_response->cmd_code = data[2] << 8 | data[3];
    uint16_t payload_len = data[4] << 8 | data[5];
    // check that the response is ok or not
    //, if not let the application layer handle this.
    if (MROVER_RC_OK != p_response->return_code)
    {
        return_status = API_PROCESSOR_SUCCESS;
        return  return_status;
    }

    switch(p_response->cmd_code)
    {   
        case MROVER_CC_GET_EVENT:
        return_status = api_processor_parse_get_event(mcm_module,&data[6],payload_len,p_response);
        break;
        case MROVER_CC_GET_VERSION:
        return_status = api_processor_parse_get_version(mcm_module,&data[6],payload_len,p_response);
        break;
        case MROVER_CC_RESET:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"reset");
        break;
        case MROVER_CC_FACTORY_RESET:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Factory reset");
        break;
        case MROVER_CC_SWITCH_NETWORK:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Switch network");
        break;
        case MROVER_CC_INIT_LORAWAN:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Init Lorawan");
        break;
        case MROVER_CC_SET_JOIN_EUI:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Set Join Eui");
        break;
        case MROVER_CC_SET_DEV_EUI:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Set Dev Eui");
        break;
        case MROVER_CC_SET_NW_KEY:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Set Network key");
        break;
        case MROVER_CC_GET_DEV_EUI:
        return_status = api_processor_parse_eui_cmd(mcm_module,&data[6],payload_len,p_response,"Dev eui");
        break;
        case MROVER_CC_GET_JOIN_EUI:
        return_status = api_processor_parse_eui_cmd(mcm_module,&data[6],payload_len,p_response,"Join eui");
        break;
        case MROVER_CC_JOIN_LORAWAN:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Join Lorawan");
        break;
        case MROVER_CC_REQUEST_UPLINK:
        return_status = api_processor_handle_request_uplink(mcm_module, &data[6], payload_len, p_response);
        break;
        case MROVER_CC_LEAVE_LORAWAN_NETWORK:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Leave Lorawan Network");
        break;
        case MROVER_CC_STOP_SID_LORAWAN_NETWORK:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Stop Sidewalk Network");
        break;
        case MROVER_CC_BLE_LINK_REQUEST:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"BLE Link Request");
        break;
        case MROVER_CC_BLE_CONNECTION_REQUEST:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"BLE Connection Request");
        break;
        case MROVER_CC_FSK_LINK_REQUEST:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"BLE FSK Link Request");
        break;
        case MROVER_CC_CSS_LINK_REQUEST:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"CSS Link Request");
        break;
        case MROVER_CC_SET_CSS_PWR_PROFILE:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Set CSS Power Profile");
        break;
        case MROVER_CC_SET_FILTERING_DOWNLINK_SIDEWALK:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Set downlink filtering command");
        break;
        case MROVER_CC_SET_LORAWAN_CLASS:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Set New class");
        break;
        case MROVER_CC_GET_LORAWAN_CLASS:
        return_status = api_processor_parse_get_class_cmd(mcm_module,&data[6],payload_len,p_response,"Get Class status");
        break;
        case MROVER_CC_START_FILE_TRANSFER:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module,&data[6],payload_len,p_response,"Start File Transfer");
        break;
        case MROVER_CC_TRIGGER_FW_UPDATE:
        return_status = api_processor_parse_cmd_with_len_zero(mcm_module, &data[6], payload_len, p_response, "Trigger FW Update");
        break;
        case MROVER_CC_FILE_STATUS:
        return_status = api_processor_parse_get_file_status(mcm_module, &data[6], payload_len, p_response);
        break;

        default:
            TRACE_INFO("Wrong type of the command code received\n");
            return_status = API_PROCESSOR_ERROR;
        break;

    }

    return return_status;
}

/**
 * @brief This function parses the data for the event GET_EVENT_TX_STATUS.
 *
 * @param[in] mcm_module Pointer to the MCM module structure.
 * @param[in] data Pointer to the data buffer containing the event data.
 * @param[in] len Length of the data buffer containing the event data.
 * @param[out] p_response Pointer to the api_processor_response_t structure
 *                       that needs to be filled up.
 *
 * @return API_PROCESSOR_SUCCESS if the response is parsed successfully,
 *         otherwise appropriate error code.
 */
static api_processor_status_t api_processor_parse_get_event_reset(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == data)
        {
            TRACE_INFO("In get event reset data is null\n");
            break;
        }
        if(2 != len)
        {
            TRACE_INFO("In get event reset data length is not 2\n");
            break;
        }
        p_response->cmd_response_data.get_event_data.get_event_data_value.reset_data.reset_count = (data[0] << 8 | data[1]);
        return_status = API_PROCESSOR_SUCCESS;
    }while(0);

    return return_status;
}

/**
 * @brief This function parses the response frame of the event GET_EVENT command
 * 
 * @param[in] mcm_module pointer to the MCM module object
 * @param[in] data pointer to the buffer which contains the response frame
 * @param[in] len length of the response frame
 * @param[out] p_response pointer to the structure of the api_processor_response_t
 * 
 * @retval API_PROCESSOR_SUCCESS if the response frame is parsed successfully
 * @retval API_PROCESSOR_ERROR if there is any error in parsing
 * 
 */
static api_processor_status_t api_processor_parse_get_event(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len,api_processor_response_t *p_response)
{
    api_processor_status_t return_status = API_PROCESSOR_SUCCESS;
    p_response->cmd_response_data.get_event_data.get_event_code = data[0];
    mcm_module->_no_of_curr_pen_evt = data[1];
    len = len - 2;

    switch (p_response->cmd_response_data.get_event_data.get_event_code)
    {
        case MODEM_EVENT_RESET:
            TRACE_INFO("MODEM_EVENT_RESET\n");
            return_status = api_processor_parse_get_event_reset(mcm_module,&data[2],len, p_response);
            break;

        case MODEM_EVENT_ALARM:
            TRACE_INFO("MODEM_EVENT_ALARM\n");
            break;

        case MODEM_EVENT_JOINED:
            TRACE_INFO("MODEM_EVENT_JOINED\n");
            break;

        case MODEM_EVENT_TXDONE:
            TRACE_INFO("MODEM_EVENT_TXDONE\n");
            return_status = api_processor_get_event_tx_status(mcm_module,&data[2],len, p_response);
            break;

        case MODEM_EVENT_DOWNDATA:
            TRACE_INFO("MODEM_EVENT_DOWNDATA\n");
            return_status = api_processor_get_event_down_data(mcm_module,&data[2],len, p_response);
            break;

        case MODEM_EVENT_UPLOADDONE:
            TRACE_INFO("MODEM_EVENT_UPLOADDONE\n");
            break;

        case MODEM_EVENT_SETCONF:
            TRACE_INFO("MODEM_EVENT_SETCONF\n");
            break;

        case MODEM_EVENT_MUTE:
            TRACE_INFO("MODEM_EVENT_MUTE\n");
            break;

        case MODEM_EVENT_STREAMDONE:
            TRACE_INFO("MODEM_EVENT_STREAMDONE\n");
            break;

        case MODEM_EVENT_JOINFAIL:
            TRACE_INFO("MODEM_EVENT_JOINFAIL\n");
            break;

        case MODEM_EVENT_TIME:
            TRACE_INFO("MODEM_EVENT_TIME\n");
            break;

        case MODEM_EVENT_TIMEOUT_ADR_CHANGED:
            TRACE_INFO("MODEM_EVENT_TIMEOUT_ADR_CHANGED\n");
            break;

        case MODEM_EVENT_NEW_LINK_ADR:
            TRACE_INFO("MODEM_EVENT_NEW_LINK_ADR\n");
            break;

        case MODEM_EVENT_LINK_CHECK:
            TRACE_INFO("MODEM_EVENT_LINK_CHECK\n");
            break;

        case MODEM_EVENT_ALMANAC_UPDATE:
            TRACE_INFO("MODEM_EVENT_ALMANAC_UPDATE\n");
            break;

        case MODEM_EVENT_USER_RADIO_ACCESS:
            TRACE_INFO("MODEM_EVENT_USER_RADIO_ACCESS\n");
            break;

        case MODEM_EVENT_CLASS_B_PING_SLOT_INFO:
            TRACE_INFO("MODEM_EVENT_CLASS_B_PING_SLOT_INFO\n");
            break;

        case MODEM_EVENT_CLASS_B_STATUS:
            TRACE_INFO("MODEM_EVENT_CLASS_B_STATUS\n");
            break;

        case MODEM_EVENT_LORAWAN_MAC_TIME:
            TRACE_INFO("MODEM_EVENT_LORAWAN_MAC_TIME\n");
            break;

        case MODEM_EVENT_SEGMENTED_FILE_DOWNLOAD:
            TRACE_INFO("MODEM_EVENT_SEGMENTED_FILE_DOWNLOAD\n");
            return_status = api_processor_parse_get_event_seg(mcm_module, &data[2], len, p_response);
            break;

        case MODEM_EVENT_CLASS_SWITCHED:
            TRACE_INFO("MODEM_EVENT_CLASS_SWITCHED\n");
            return_status = api_procesor_parse_lorawan_class_switch(mcm_module,&data[2],len, p_response);
            break;

        case MODEM_EVENT_NONE:
            TRACE_INFO("MODEM_EVENT_NONE\n");
            return_status = API_PROCESSOR_SUCCESS;
            break;

        default:
            TRACE_INFO("Wrong type of the modem event code\n");
            return_status = API_PROCESSOR_ERROR;
            break;
    }

    return return_status;
}


/**
 * @brief This function parses the data for the event GET_EVENT_TX_STATUS.
 *
 * @param[in] mcm_module Pointer to the MCM module structure.
 * @param[in] data Pointer to the data buffer containing the event data.
 * @param[in] len Length of the data buffer containing the event data.
 * @param[out] p_response Pointer to the api_processor_response_t structure
 *                       that needs to be filled up.
 *
 * @return API_PROCESSOR_SUCCESS if the response is parsed successfully,
 *         otherwise appropriate error code.
 */
static api_processor_status_t api_processor_get_event_tx_status(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == data || 1 != len)
        {
            TRACE_INFO("Invalid data payload for get event tx status\n");
            break;
        }
        if(MROVER_TX_DONE_WITH_ACK < data[0])
        {
            TRACE_INFO("Got the invalid tx status value 0x%02x\n",data[0]);
            break;
        }
        p_response->cmd_response_data.get_event_data.get_event_data_value.tx_status_data.tx_status = data[0];
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);
        


    return return_status;
}

/**
 * @brief This function parses the data for the event GET_EVENT_DOWN_DATA.
 *
 * @param[in] mcm_module Pointer to the MCM module structure.
 * @param[in] data Pointer to the data buffer containing the event data.
 * @param[in] len Length of the data buffer containing the event data.
 * @param[out] p_response Pointer to the api_processor_response_t structure
 *                       that needs to be filled up.
 *
 * @return API_PROCESSOR_SUCCESS if the response is parsed successfully,
 *         otherwise appropriate error code.
 */
static api_processor_status_t api_processor_get_event_down_data(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == data || MIN_DOWNLINK_PAYLOAD_LEN > len)
        {
            TRACE_INFO("Invalid data payload for get event downlink\n");
            break;
        }
        // parse according to the data type, either lorawan or sidewalk

        if(COMMAND_TYPE_LORAWAN == p_response->cmd_type)
        {
            // parse for the lorawan
            return_status = api_processor_parse_lorawan_down_data(mcm_module, data, len, p_response);
        }
        // no need to validate, as we already validated the command type in the frame before
        else 
        {
           return_status = api_processor_parse_sid_down_data(mcm_module,data,len,p_response);
        }
    } while (0);
    

    return return_status;
}

/**
 * @brief This function parses the downlink data for either lorawan or sidewalk
 * depending on the command type in the response. The downlink data contains 
 * RSSI and SNR values for the downlink reception
 * 
 * @param[in] mcm_module pointer to the mcm module
 * @param[in] data pointer to the data received
 * @param[in] len length of the data
 * @param[out] p_response structure containing the response of the command
 * 
 * @return API_PROCESSOR_SUCCESS if the parsing is successful. If any error
 * in parsing, returns API_PROCESSOR_ERROR
 * 
 */
static api_processor_status_t api_processor_parse_lorawan_down_data(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {   
        p_response->cmd_response_data.get_event_data.get_event_data_value.down_data.rssi = data[0];
        p_response->cmd_response_data.get_event_data.get_event_data_value.down_data.snr = data[1];
        p_response->cmd_response_data.get_event_data.get_event_data_value.down_data.lrwan_sid_seq_port= data[2];
        p_response->cmd_response_data.get_event_data.get_event_data_value.down_data.payload = data + 3;
        p_response->cmd_response_data.get_event_data.get_event_data_value.down_data.payload_len = len - 3;
        
        return_status = API_PROCESSOR_SUCCESS;

    }while (0);

    return return_status;
}


static api_processor_status_t api_procesor_parse_lorawan_class_switch(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do 
    {
        p_response->cmd_response_data.get_event_data.get_event_data_value.class_switch_data.new_class = data[0];
        return_status = API_PROCESSOR_SUCCESS;

    }while (0);

    return return_status;
}

static api_processor_status_t api_processor_parse_get_event_seg(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (len!=sizeof(get_seg_file_status_t))
        {
            return_status = API_PROCESSOR_INVALID_SERIAL_DATA;
            break;
        }
        

        p_response->cmd_response_data.get_event_data.get_event_data_value.download_segment_data.cmd_type.bin_type = data[0] & 0x0F;
        p_response->cmd_response_data.get_event_data.get_event_data_value.download_segment_data.fw_ver.major = data[1];
        p_response->cmd_response_data.get_event_data.get_event_data_value.download_segment_data.fw_ver.minor = data[2];
        p_response->cmd_response_data.get_event_data.get_event_data_value.download_segment_data.fw_ver.patch = data[3];

        p_response->cmd_response_data.get_event_data.get_event_data_value.download_segment_data.pkg_size[0] = data[4];
        p_response->cmd_response_data.get_event_data.get_event_data_value.download_segment_data.pkg_size[1] = data[5];
        p_response->cmd_response_data.get_event_data.get_event_data_value.download_segment_data.pkg_size[2] = data[6];
        p_response->cmd_response_data.get_event_data.get_event_data_value.download_segment_data.seg_size = data[7] & 0x0F;
        p_response->cmd_response_data.get_event_data.get_event_data_value.download_segment_data.nxt_seg_id = data[7] >> 4;
        p_response->cmd_response_data.get_event_data.get_event_data_value.download_segment_data.seg_status = data[9] << 8 | data[8];
       
        

        return_status = API_PROCESSOR_SUCCESS;

    }while (0);

    return return_status;
}

/**
 * @brief This function parses the downlink data for sidewalk.
 * 
 * @param[in] mcm_module pointer to the mcm module
 * @param[in] data pointer to the data received
 * @param[in] len length of the data
 * @param[out] p_response structure containing the response of the command
 * 
 * @return API_PROCESSOR_SUCCESS if the parsing is successful. If any error
 * in parsing, returns API_PROCESSOR_ERROR
 * 
 */
static api_processor_status_t api_processor_parse_sid_down_data(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {   
        
        p_response->cmd_response_data.get_event_data.get_event_data_value.down_data.lrwan_sid_seq_port = (data[0] << 8) | data[1];
        p_response->cmd_response_data.get_event_data.get_event_data_value.down_data.rssi = data[2];
        p_response->cmd_response_data.get_event_data.get_event_data_value.down_data.snr = data[3];
        p_response->cmd_response_data.get_event_data.get_event_data_value.down_data.payload = data + 4;
        p_response->cmd_response_data.get_event_data.get_event_data_value.down_data.payload_len = len - 4;

        return_status = API_PROCESSOR_SUCCESS;
    } while (0);
    

    return return_status;
}


/**
 * @brief This function parses the version command response, which is the
 * version number of the device
 * 
 * @param[in] mcm_module pointer to the mcm module
 * @param[in] data pointer to the data received
 * @param[in] len length of the data
 * @param[out] p_response structure containing the response of the command
 * 
 * @return API_PROCESSOR_SUCCESS if the parsing is successful. If any error
 * in parsing, returns API_PROCESSOR_ERROR
 * 
 */
static api_processor_status_t api_processor_parse_get_version(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if((NULL == data) || (GET_VERSION_RESPONSE_PAYLOAD_LEN != len) )
        {
            TRACE_INFO("Invalid data payload for get version\n");
            break;
        }        
        p_response->cmd_response_data.ver_info.bootloader.major = data[0];
        p_response->cmd_response_data.ver_info.bootloader.minor = data[1];
        p_response->cmd_response_data.ver_info.bootloader.patch = (data[2] << 8) | data[3];

        p_response->cmd_response_data.ver_info.modem_fw.major = data[4];
        p_response->cmd_response_data.ver_info.modem_fw.minor = data[5];
        p_response->cmd_response_data.ver_info.modem_fw.patch = (data[6] << 8) | data[7];
        
        p_response->cmd_response_data.ver_info.modem_hw.major = data[8];
        p_response->cmd_response_data.ver_info.modem_hw.minor = data[9];
        p_response->cmd_response_data.ver_info.modem_hw.patch = data[10];
        
        p_response->cmd_response_data.ver_info.sidewalk.major = data[11];
        p_response->cmd_response_data.ver_info.sidewalk.minor = data[12];
        p_response->cmd_response_data.ver_info.sidewalk.patch = data[13];
        
        p_response->cmd_response_data.ver_info.lorawan.major = data[14];
        p_response->cmd_response_data.ver_info.lorawan.minor = data[15];
        p_response->cmd_response_data.ver_info.lorawan.patch = data[16];

        return_status = API_PROCESSOR_SUCCESS;

    } while (0);
    

    return return_status;

}

static api_processor_status_t api_processor_parse_get_file_status(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if((NULL == data) || (sizeof(get_seg_file_status_t) != len) )
        {
            TRACE_INFO("Invalid data payload for get file status\n");
            break;
        }
        p_response->cmd_response_data.seg_file_status.cmd_type.bin_type = data[0] & 0x0F;
        p_response->cmd_response_data.seg_file_status.fw_ver.major = data[1];
        p_response->cmd_response_data.seg_file_status.fw_ver.minor = data[2];
        p_response->cmd_response_data.seg_file_status.fw_ver.patch = data[3];
        p_response->cmd_response_data.seg_file_status.pkg_size[0] = data[4];
        p_response->cmd_response_data.seg_file_status.pkg_size[1] = data[5];
        p_response->cmd_response_data.seg_file_status.pkg_size[2] = data[6];
        p_response->cmd_response_data.seg_file_status.seg_size = data[7] & 0x0F;
        p_response->cmd_response_data.seg_file_status.nxt_seg_id = data[7] >> 4;
        p_response->cmd_response_data.seg_file_status.seg_status = data[9] << 8 | data[8];

        return_status = API_PROCESSOR_SUCCESS;

    } while (0);


    return return_status;

}


/**
 * 
 * 
 * @brief This function is used to parse the data of command which have zero length of payload.
 *        This function fills up the response structure with the command type,
 *        command and command response status.
 *
 * @param mcm_module Pointer to the MCM module structure.
 * @param data Pointer to the data buffer containing the data.
 * @param len Length of the data buffer containing the data.
 * @param p_response Pointer to the api_processor_response_t structure
 *                   that needs to be filled up.
 * @param module Name of the module.
 *
 * @return API_PROCESSOR_SUCCESS if the parsing is successful. If any error
 *         in parsing, returns API_PROCESSOR_ERROR
 */
static api_processor_status_t api_processor_parse_cmd_with_len_zero(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response,const char* module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
     do
    {
        if(0 != len)
        {
            TRACE_INFO("Wrong payload length for %s command\n",module);
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);
    
    return return_status;

}
/**
 * @brief This function is used to parse the data of command which have
 *        length of payload zero. It fills up the response structure with
 *        the command type, command and command response status.
 *
 * @param mcm_module Pointer to the MCM module structure.
 * @param data Pointer to the data buffer containing the data.
 * @param len Length of the data buffer containing the data.
 * @param p_response Pointer to the api_processor_response_t structure
 *                   that needs to be filled up.
 * @param module Name of the module.
 *
 * @return API_PROCESSOR_SUCCESS if the parsing is successful. If any error
 *         in parsing, returns API_PROCESSOR_ERROR
 */
static api_processor_status_t api_processor_parse_eui_cmd(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response,const char* eui_type)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if(LORAWAN_DEV_EUI_JOIN_EUI_LEN != len)
        {
            TRACE_INFO("Wrong payload length for %s command\n",eui_type);
            break;
        }
        if(MROVER_CC_GET_DEV_EUI == p_response->cmd_code)
        {
            p_response->cmd_response_data.dev_eui = data;
           
        }
        else
        {
            p_response->cmd_response_data.join_eui = data;
            
        }

        return_status = API_PROCESSOR_SUCCESS;

    } while (0);
    

    return  return_status;
}

static api_processor_status_t api_processor_parse_get_class_cmd(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response,const char* eui_type)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if(1 != len)
        {
            TRACE_INFO("Wrong payload length for %s command\n",eui_type);
            break;
        }

        p_response->cmd_response_data.get_event_data.get_event_data_value.class_switch_data.new_class = data[0];

        return_status = API_PROCESSOR_SUCCESS;

    } while (0);
    

    return  return_status;
}

static api_processor_status_t api_processor_parse_cmd(mcm_module_hdl_t *mcm_module,uint8_t *data, uint16_t len, api_processor_response_t *p_response,const char* eui_type)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if(LORAWAN_DEV_EUI_JOIN_EUI_LEN != len)
        {
            TRACE_INFO("Wrong payload length for %s command\n",eui_type);
            break;
        }
        if(MROVER_CC_GET_DEV_EUI == p_response->cmd_code)
        {
            p_response->cmd_response_data.dev_eui = data;
           
        }
        else
        {
            p_response->cmd_response_data.join_eui = data;
            
        }

        return_status = API_PROCESSOR_SUCCESS;

    } while (0);
    

    return  return_status;
}

/**
 * @brief This function parses the single frame that is received from the
 *        MCM module. It populates the api_processor_response_t structure with
 *        the data and returns the status of the parsing.
 *
 * @param mcm_module Pointer to the MCM module structure.
 * @param data Pointer to the data buffer containing the data.
 * @param len Length of the data buffer containing the data.
 *
 * @return API_PROCESSOR_SUCCESS if the parsing is successful. If any error
 *         in parsing, returns API_PROCESSOR_ERROR
 */
static api_processor_status_t api_processor_parse_single_frame(mcm_module_hdl_t *mcm_module, uint8_t* data,uint16_t len)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (mcm_module == NULL)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        if (data == NULL || len < MIN_RX_PAYLOAD_LEN)
        {   
            TRACE_INFO("Serial data is not valid. Length: %d\n", len);
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        
        bool is_frame_notification = fp_is_frame_notification(data, len);
        TRACE_INFO("Frame notification check: %s\n", is_frame_notification ? "True" : "False");

        if (is_frame_notification)
        {
            // Validate and parse for notification
            fp_api_status_t status = fp_is_valid_notify_frame(data, len);
            if (status != FP_SUCCESS)
            {
                TRACE_INFO("Failed to parse Notification\n");
                break;
            }
            // Get the value of the pending event
            mcm_module->_no_of_curr_pen_evt = fp_get_pending_event_count(data, len);
            TRACE_INFO("Pending event count: %d\n", mcm_module->_no_of_curr_pen_evt);
            mcm_module->handle_notification_cb(mcm_module->user_context);
            return_status = API_PROCESSOR_SUCCESS;
            break;
        }
        else
        {
            // Parse the data
            fp_api_status_t status = fp_is_valid_response_frame(data, len);
            if (status != FP_SUCCESS)
            {
                TRACE_INFO("Failed to parse data\n");
                break;
            }

            api_processor_response_t response;
            // Now parse the response frame
            return_status = api_processor_parse_response_frame(mcm_module, data, len, &response);

            // In case of parsing error, just return error
            if (return_status != API_PROCESSOR_SUCCESS)
            {
                TRACE_INFO("Response parsing failed with status: %d\n", return_status);
                break;
            }

            // Now call the callback function for response
            TRACE_INFO("Calling response callback\n");
            mcm_module->handle_response_cb(&response, mcm_module->user_context);
        }

    } while (0);
    
    return return_status;
}


/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/

/*******************************************************************************
* Helper Functions
********************************************************************************/

inline mrover_return_code_t mcm_helper_get_response_code(const api_processor_response_t *res)
{
    return res->return_code;
}

inline command_types_t mcm_helper_get_command_type(const api_processor_response_t *res)
{
    return res->cmd_type;
}

inline mrover_cc_codes_t mcm_helper_get_command_code(const api_processor_response_t *res)
{
    return res->cmd_code;
}

inline const uint8_t* mcm_helper_get_dev_eui(const api_processor_response_t *res)
{
    return res->cmd_response_data.dev_eui;
}

inline const uint8_t* mcm_helper_get_join_eui(const api_processor_response_t *res)
{
   return res->cmd_response_data.join_eui;
}

inline mrover_lorawan_class_t mcm_helper_get_device_class(const api_processor_response_t *res)
{
    return res->cmd_response_data.get_event_data.get_event_data_value.class_switch_data.new_class;
}

void mcm_helper_get_version(const api_processor_response_t *res,ver_type_2_t *bootloader,ver_type_2_t* mdm_fw,\
                            ver_type_1_t *mdm_hw,ver_type_1_t *sidwlk,ver_type_1_t *lrwan)
{
    *bootloader = res->cmd_response_data.ver_info.bootloader;
    *mdm_fw = res->cmd_response_data.ver_info.modem_fw;
    *mdm_hw = res->cmd_response_data.ver_info.modem_hw;
    *sidwlk = res->cmd_response_data.ver_info.sidewalk;
    *lrwan = res->cmd_response_data.ver_info.lorawan;
}

void mcm_helper_get_downlink_data(const api_processor_response_t *res,int8_t *rssi,int8_t *snr,uint8_t *payload,uint16_t *port_seq)
{
    *rssi = res->cmd_response_data.get_event_data.get_event_data_value.down_data.rssi;
    *snr = res->cmd_response_data.get_event_data.get_event_data_value.down_data.snr;
    *port_seq = res->cmd_response_data.get_event_data.get_event_data_value.down_data.lrwan_sid_seq_port;
    if(NULL != payload)
    {
        memcpy(payload,res->cmd_response_data.get_event_data.get_event_data_value.down_data.payload,res->cmd_response_data.get_event_data.get_event_data_value.down_data.payload_len);   
    }
}

inline uint16_t mcm_helper_get_downlink_len(const api_processor_response_t *res)
{
    return res->cmd_response_data.get_event_data.get_event_data_value.down_data.payload_len;
}

inline uint8_t mcm_helper_get_event_reset_count(const api_processor_response_t *res)
{
    return res->cmd_response_data.get_event_data.get_event_data_value.reset_data.reset_count;
}


inline get_event_code_t mcm_helper_get_event_code(const api_processor_response_t *res)
{
    return res->cmd_response_data.get_event_data.get_event_code;
}

inline mrover_uplink_event_type_t mcm_helper_get_event_tx_status(const api_processor_response_t *res)
{
    return res->cmd_response_data.get_event_data.get_event_data_value.tx_status_data.tx_status;
}

inline mrover_lorawan_class_t mcm_helper_get_event_new_class(const api_processor_response_t *res)
{
    return res->cmd_response_data.get_event_data.get_event_data_value.class_switch_data.new_class;
}

inline get_seg_file_status_t mcm_helper_get_event_seg_down(const api_processor_response_t *res)
{
    return res->cmd_response_data.get_event_data.get_event_data_value.download_segment_data;
}

void mcm_helper_get_seg_file_status(const api_processor_response_t *res, get_seg_file_status_t *seg_file_status)
{
    *seg_file_status = res->cmd_response_data.seg_file_status;
}
/******************************************************************************
 * Function Definitions
 *******************************************************************************/

api_processor_status_t api_processor_init(mcm_module_hdl_t *mcm_module,serial_send_data_cb send_data_cb,mrover_notification_cb h_mrover_notification_cb,mrover_response_cb h_mrover_response_cb)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    

    do
    {
        if (NULL == mcm_module)
        {
            TRACE_INFO("mcm_module is NULL\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }

        if (NULL == send_data_cb)
        {
            TRACE_INFO("Please profile the callback function for serial send data\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }
        mcm_module->h_serial_device.send_data_cb = send_data_cb;

        if(NULL == h_mrover_notification_cb)
        {
            TRACE_INFO("Warn, Notification callback not set\n");
            break;
        }

        mcm_module->handle_notification_cb = h_mrover_notification_cb;
        if(NULL == h_mrover_response_cb)
        {
            TRACE_INFO("MCM module response call back handle is not provided\n");
            break;
        }

        mcm_module->handle_response_cb = h_mrover_response_cb;
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}



void api_processor_get_lib_ver(ver_type_1_t *ver) {
    if (ver != NULL) 
    {
        ver->major = API_PROCESSOR_LIB_MAJOR_VERSION; // Set major version
        ver->minor = API_PROCESSOR_LIB_MINOR_VERSION; // Set minor version
        ver->patch = API_PROCESSOR_LIB_PATCH_VERSION; // Set patch version
    }
}


api_processor_status_t api_processor_cmd_get_event(mcm_module_hdl_t *mcm_module)
{
    const uint16_t max_payload_size = 6;
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_GENERAL;
        mcm_module->u8_send_payload[1] = MROVER_CC_GET_EVENT >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_GET_EVENT & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_get_version(mcm_module_hdl_t *mcm_module)
{
    const uint16_t max_payload_size = 6;
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_GENERAL;
        mcm_module->u8_send_payload[1] = MROVER_CC_GET_VERSION >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_GET_VERSION & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_reset(mcm_module_hdl_t *mcm_module)
{

    const uint16_t max_payload_size = 6;
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_GENERAL;
        mcm_module->u8_send_payload[1] = MROVER_CC_RESET >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_RESET & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_factory_reset(mcm_module_hdl_t *mcm_module)
{
    const uint16_t max_payload_size = 6;
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);

        mcm_module->u8_send_payload[0] = COMMAND_TYPE_GENERAL;
        mcm_module->u8_send_payload[1] = MROVER_CC_FACTORY_RESET >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_FACTORY_RESET & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_switch_network(mcm_module_hdl_t *mcm_module)
{
    const uint16_t max_payload_size = 6;
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_GENERAL;
        mcm_module->u8_send_payload[1] = MROVER_CC_SWITCH_NETWORK >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_SWITCH_NETWORK & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;
        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_init_lorawan(mcm_module_hdl_t *mcm_module)
{
    const uint16_t max_payload_size = 6;
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {

        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);

        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;
        mcm_module->u8_send_payload[1] = MROVER_CC_INIT_LORAWAN >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_INIT_LORAWAN & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;
        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_set_join_eui(mcm_module_hdl_t *mcm_module, uint8_t *p_join_eui, uint8_t u8_join_eui_len)
{
    const uint16_t max_payload_size = 14;
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        if (u8_join_eui_len != 8 || p_join_eui == NULL)
        {
            TRACE_INFO("Join eui is not correct. Please provide a valid join eui\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);

        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;
        mcm_module->u8_send_payload[1] = MROVER_CC_SET_JOIN_EUI >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_SET_JOIN_EUI & 0xFF;
        mcm_module->u8_send_payload[3] = 0x00;
        mcm_module->u8_send_payload[4] = u8_join_eui_len;
        memcpy(&mcm_module->u8_send_payload[5], p_join_eui, u8_join_eui_len);

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;

    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_set_dev_eui(mcm_module_hdl_t *mcm_module, uint8_t *p_dev_eui, uint8_t u8_dev_eui_len)
{
    const uint16_t max_payload_size = 14;
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        if (u8_dev_eui_len != 8 || p_dev_eui == NULL)
        {
            TRACE_INFO("Dev eui is not correct. Please provide a valid dev eui\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);

        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;
        mcm_module->u8_send_payload[1] = MROVER_CC_SET_DEV_EUI >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_SET_DEV_EUI & 0xFF;
        mcm_module->u8_send_payload[3] = 0x00;
        mcm_module->u8_send_payload[4] = u8_dev_eui_len;
        memcpy(&mcm_module->u8_send_payload[5], p_dev_eui, u8_dev_eui_len);

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;

    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_set_nwk_key(mcm_module_hdl_t *mcm_module, uint8_t *p_nwk_key, uint16_t u16_nwk_key_len)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    const uint16_t max_payload_size = 22;
    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        if (p_nwk_key == NULL)
        {
            TRACE_INFO("Please provide a valid network key\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }

        if (16 != u16_nwk_key_len)
        {
            TRACE_INFO("Network key length must be 16 bytes\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;
        mcm_module->u8_send_payload[1] = MROVER_CC_SET_NW_KEY >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_SET_NW_KEY & 0xFF;
        mcm_module->u8_send_payload[3] = 0x00;
        mcm_module->u8_send_payload[4] = u16_nwk_key_len;
        memcpy(&mcm_module->u8_send_payload[5], p_nwk_key, u16_nwk_key_len);

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_get_dev_eui(mcm_module_hdl_t *mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    const uint16_t max_payload_size = 6;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;
        mcm_module->u8_send_payload[1] = MROVER_CC_GET_DEV_EUI >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_GET_DEV_EUI & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;

    } while (0);

    return return_status;
}

api_processor_status_t api_processor_cmd_get_join_eui(mcm_module_hdl_t *mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    const uint16_t max_payload_size = 6;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);

        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;
        mcm_module->u8_send_payload[1] = MROVER_CC_GET_JOIN_EUI >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_GET_JOIN_EUI & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;

    } while (0);

    return return_status;
}

api_processor_status_t api_processor_cmd_join_lorawan(mcm_module_hdl_t *mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    const uint16_t max_payload_size = 6;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;
        mcm_module->u8_send_payload[1] = MROVER_CC_JOIN_LORAWAN >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_JOIN_LORAWAN & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;

    } while (0);

    return return_status;
}

/**
 * @brief 'Request Uplink' command requests sending the given data on the specified port as an
        unconfirmed or confirmed frame. The payload of the request uplink specifies the confirmed or
        unconfirmed frame as below -
        a. MROVER_UPLINK_UNCONFIRMED - unconfirmed data
        b. MROVER_UPLINK_CONFIRMED - confirmed data
 * @note 1. The application shall not use port 0 or the ports from 225 to 255 since they are reserved
            for future standardized application extensions.
 * @param mcm_module MCM module handle
 * @param u8_port Port number on which data needs to be sent
 * @param u8_payload Pointer to the payload to be sent
 * @param u16_payload_size Length of the payload to be sent
 * @param h_uplink_type Type of the uplink frame - unconfirmed or confirmed
 * @return API_PROCESSOR_SUCCESS on success, API_PROCESSOR_ERROR on error.
 */
api_processor_status_t api_processor_cmd_request_lorawan_uplink(mcm_module_hdl_t *mcm_module,uint8_t u8_port,uint8_t *u8_payload,uint16_t u16_payload_size,mrover_uplink_type_t h_uplink_type)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    //validation the input data 
    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        if(LORAWAN_TX_MAX_PAYLOAD_SIZE < u16_payload_size)
        {
            TRACE_INFO("Data payload cannot be exceeded more than 255 bytes\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }

        // The application shall not use port 0 or the ports from 225 to 255 since they are reserved
        //for future standardized application extensions
        if((0 == u8_port) || (225 <= u8_port))
        {
            TRACE_INFO("Please provide a valid port number\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }

        if((NULL != u8_payload) && (0 == u16_payload_size)) // if data is present but size is zero
        {
            TRACE_INFO("Please provide a valid payload\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }

        if((NULL == u8_payload) && (0 != u16_payload_size)) // if data is not present but size is not zero
        {
            TRACE_INFO("Please provide a valid payload\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }

        if(MROVER_CONFIRMED_UPLINK < h_uplink_type)
        {
            TRACE_INFO("Please provide a valid uplink type\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }

        uint16_t u16_payload_len = u16_payload_size + 2; // 1 bytes for port number and 1 byte for uplink type
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;
        mcm_module->u8_send_payload[1] = MROVER_CC_REQUEST_UPLINK >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_REQUEST_UPLINK & 0xFF;
        mcm_module->u8_send_payload[3] = u16_payload_len >> 8;
        mcm_module->u8_send_payload[4] = u16_payload_len & 0xFF;
        mcm_module->u8_send_payload[5] = u8_port;
        mcm_module->u8_send_payload[6] = h_uplink_type;
        memcpy(&mcm_module->u8_send_payload[7],u8_payload,u16_payload_size);

        const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN + u16_payload_len;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;

    } while (0);
    
    return return_status;
}


api_processor_status_t api_processor_cmd_leave_lorawan_network(mcm_module_hdl_t* mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
     const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN;
    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;
        mcm_module->u8_send_payload[1] = MROVER_CC_LEAVE_LORAWAN_NETWORK >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_LEAVE_LORAWAN_NETWORK & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}

api_processor_status_t api_processor_cmd_stop_lorawan_network(mcm_module_hdl_t* mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN + 1; // extra byte for the payload
    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;
        mcm_module->u8_send_payload[1] = MROVER_CC_STOP_SID_LORAWAN_NETWORK >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_STOP_SID_LORAWAN_NETWORK & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0x01;
        mcm_module->u8_send_payload[5] = 0x01;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_sid_ble_link_request(mcm_module_hdl_t* mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN; // extra byte for the payload

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_SIDEWALK;
        mcm_module->u8_send_payload[1] = MROVER_CC_BLE_LINK_REQUEST >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_BLE_LINK_REQUEST & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_sid_ble_conn_request(mcm_module_hdl_t* mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN; // extra byte for the payload

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_SIDEWALK;
        mcm_module->u8_send_payload[1] = MROVER_CC_BLE_CONNECTION_REQUEST >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_BLE_CONNECTION_REQUEST & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_sid_fsk_link_request(mcm_module_hdl_t* mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN; // extra byte for the payload

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_SIDEWALK;
        mcm_module->u8_send_payload[1] = MROVER_CC_FSK_LINK_REQUEST >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_FSK_LINK_REQUEST & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}

api_processor_status_t api_processor_cmd_sid_css_link_request(mcm_module_hdl_t* mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN; // extra byte for the payload

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_SIDEWALK;
        mcm_module->u8_send_payload[1] = MROVER_CC_CSS_LINK_REQUEST >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_CSS_LINK_REQUEST & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 0;
        mcm_module->u8_send_payload[5] = 0;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_sid_set_css_profile(mcm_module_hdl_t* mcm_module,mrover_css_pwr_profile_t h_profile)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN + 1; // extra byte for the payload

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        if(MROVER_CC_SET_CSS_PWR_PROFILE < h_profile){
            TRACE_INFO("Invalid CSS profile\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }

        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_SIDEWALK;
        mcm_module->u8_send_payload[1] = MROVER_CC_SET_CSS_PWR_PROFILE >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_SET_CSS_PWR_PROFILE & 0xFF;
        mcm_module->u8_send_payload[3] = 0;
        mcm_module->u8_send_payload[4] = 1;
        mcm_module->u8_send_payload[5] = h_profile;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;
    } while (0);

    return return_status;
}


api_processor_status_t api_processor_cmd_sid_send_uplink(mcm_module_hdl_t* mcm_module, uint8_t *u8_payload, uint16_t u16_payload_size, mrover_uplink_type_t h_uplink_type)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        // Validate payload size based on link type
        if (SIDEWALK_TX_MAX_BLE_PAYLOAD_SIZE < u16_payload_size)
        {
            TRACE_INFO("Data payload cannot be exceed more than 255 bytes\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }

        if((NULL == u8_payload) || (0 == u16_payload_size))
        {
            TRACE_INFO("Data payload cannot be empty\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }

        if(MROVER_CONFIRMED_UPLINK < h_uplink_type)
        {
            TRACE_INFO("Please provide a valid uplink type\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break;
        }

        uint16_t u16_payload_len = u16_payload_size + 1; // 1 byte for uplink type
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_SIDEWALK;
        mcm_module->u8_send_payload[1] = MROVER_CC_REQUEST_UPLINK >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_REQUEST_UPLINK & 0xFF;
        mcm_module->u8_send_payload[3] = u16_payload_len >> 8;
        mcm_module->u8_send_payload[4] = u16_payload_len & 0xFF;
        mcm_module->u8_send_payload[5] = h_uplink_type;
        memcpy(&mcm_module->u8_send_payload[6], u8_payload, u16_payload_size);

        const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN + u16_payload_len;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;

    } while (0);
    
    return return_status;
}


api_processor_status_t api_processor_cmd_set_sid_downlink_filter(mcm_module_hdl_t* mcm_module, mrover_sid_downlink_filter_t h_filtering)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
         if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        if(MROVER_SID_DISABLE_FILTERING < h_filtering)
        {
            TRACE_INFO("Wrong filtering value\n");
            return_status = API_PROCESSOR_INVALID_PARAMETERS;
            break; 
        }

        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN + 1; // extra byte for the payload

        mcm_module->u8_send_payload[0] = COMMAND_TYPE_SIDEWALK;
        mcm_module->u8_send_payload[1] = MROVER_CC_SET_FILTERING_DOWNLINK_SIDEWALK >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_SET_FILTERING_DOWNLINK_SIDEWALK & 0xFF;
        mcm_module->u8_send_payload[3] = 0x00;
        mcm_module->u8_send_payload[4] = 0x01;
        mcm_module->u8_send_payload[5] = h_filtering;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;

    } while (0);
    
    return return_status;
}


api_processor_status_t api_processor_cmd_sid_stop(mcm_module_hdl_t* mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

     do
    {
         if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN + 1; // extra byte for the payload

        mcm_module->u8_send_payload[0] = COMMAND_TYPE_SIDEWALK;
        mcm_module->u8_send_payload[1] = MROVER_CC_STOP_SID_LORAWAN_NETWORK >> 8;
        mcm_module->u8_send_payload[2] = MROVER_CC_STOP_SID_LORAWAN_NETWORK & 0xFF;
        mcm_module->u8_send_payload[3] = 0x00;
        mcm_module->u8_send_payload[4] = 0x01;
        mcm_module->u8_send_payload[5] = SIDEWALK_STOP_DATA_PAYLOAD;

        // send the payload to the frame parser for crc check and other validation
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append crc\n");
            break;
        }

        // send the data to the module for sending
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(mcm_module->u8_send_payload, max_payload_size,mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }
        return_status = API_PROCESSOR_SUCCESS;

    } while (0);
    
    return return_status;

}

//TODO Oxit: Paresh: Rename file to something more relevant

api_processor_status_t api_processor_parse_rx_data(mcm_module_hdl_t *mcm_module,uint8_t* data,uint16_t len)
{   
    api_processor_status_t return_status = API_PROCESSOR_ERROR;
    TRACE_INFO("Parsing RX data: length = %d\n", len); // Debug print for data length

    // TODO: currently we are only handling for 2 concurrent frame, can be done for the multiple frame also 
    // check if frame is single or multiple
    if(fp_is_single_frame(data,len))
    {
        TRACE_INFO("Single frame detected\n"); // Debug print for single frame
        return_status = api_processor_parse_single_frame(mcm_module,data,len);
    }
    else
    {
        TRACE_INFO("Multiple frame detected\n"); // Debug print for multiple frame
        // if frame is notification, then just parse first MIN_RX_PAYLOAD_LEN, as notification, rest as the response
        if(MROVER_RC_NOTIFY_EVENTS == data[0])
        {   
            TRACE_INFO("Notification frame detected\n"); // Debug print for notification
            // parser for notification
            return_status = api_processor_parse_single_frame(mcm_module,data,MIN_RX_PAYLOAD_LEN);

            if((len-MIN_RX_PAYLOAD_LEN) > MIN_RX_PAYLOAD_LEN) // if response is available
            {
                TRACE_INFO("Parsing response frame\n"); // Debug print for response
                // parser for response
                return_status = api_processor_parse_single_frame(mcm_module, data + MIN_RX_PAYLOAD_LEN, len - MIN_RX_PAYLOAD_LEN);
            }
        }
        else
        {
            TRACE_INFO("Response frame detected\n"); // Debug print for response frame
            // response frame first
            return_status = api_processor_parse_single_frame(mcm_module,data,(len-MIN_RX_PAYLOAD_LEN));

            if((len-MIN_RX_PAYLOAD_LEN) >= MIN_RX_PAYLOAD_LEN)
            {
                TRACE_INFO("Parsing notification after response\n"); // Debug print for notification after response
                // then notification
                data = data + (len-MIN_RX_PAYLOAD_LEN); // move the data pointer to the start of the notification
                return_status = api_processor_parse_single_frame(mcm_module, data, MIN_RX_PAYLOAD_LEN);
            }
        }
    }
    TRACE_INFO("Finished parsing RX data with status: %d\n", return_status); // Debug print for return status
    return return_status;
}


inline uint8_t api_processor_get_pending_events(mcm_module_hdl_t *mcm_module)
{
    return mcm_module->_no_of_curr_pen_evt;
}

api_processor_status_t api_processor_cmd_set_lorawan_class(mcm_module_hdl_t *mcm_module, 
                                                           mrover_lorawan_class_t lorawan_class)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        // Clear the payload buffer
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN + 1; // 1 byte for class payload

        // Construct the payload
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;                  // Command type
        mcm_module->u8_send_payload[1] = MROVER_CC_SET_LORAWAN_CLASS >> 8;     // Command code MSB
        mcm_module->u8_send_payload[2] = MROVER_CC_SET_LORAWAN_CLASS & 0xFF;   // Command code LSB
        mcm_module->u8_send_payload[3] = 0x00;                                 
        mcm_module->u8_send_payload[4] = 0x01;                                 
        mcm_module->u8_send_payload[5] = (uint8_t)lorawan_class;               // LoRaWAN class

        // Append CRC
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append CRC\n");
            break;
        }

        // Send the payload
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(
            mcm_module->u8_send_payload, max_payload_size, mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }

        return_status = API_PROCESSOR_SUCCESS;

    } while (0);

    return return_status;
}

api_processor_status_t api_processor_cmd_get_lorawan_class(mcm_module_hdl_t *mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        // Clear the payload buffer
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN;

        // Construct the payload
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_LORAWAN;                  // Command type
        mcm_module->u8_send_payload[1] = MROVER_CC_GET_LORAWAN_CLASS >> 8;     // Command code MSB
        mcm_module->u8_send_payload[2] = MROVER_CC_GET_LORAWAN_CLASS & 0xFF;   // Command code LSB
        mcm_module->u8_send_payload[3] = 0x00;                                 // Reserved
        mcm_module->u8_send_payload[4] = 0x00;                                 // Payload length
        mcm_module->u8_send_payload[5] = 0x00;                                 // Placeholder for optional data

        // Append CRC
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append CRC\n");
            break;
        }

        // Send the payload
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(
            mcm_module->u8_send_payload, max_payload_size, mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }

        return_status = API_PROCESSOR_SUCCESS;

    } while (0);

    return return_status;
}

api_processor_status_t api_processor_cmd_start_file_transfer(mcm_module_hdl_t *mcm_module,ver_type_1_t version)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        // Clear the payload buffer
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN + sizeof(ver_type_1_t);

        // Construct the payload
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_GENERAL;                  // Command type
        mcm_module->u8_send_payload[1] = MROVER_CC_START_FILE_TRANSFER >> 8;     // Command code MSB
        mcm_module->u8_send_payload[2] = MROVER_CC_START_FILE_TRANSFER & 0xFF;   // Command code LSB
        mcm_module->u8_send_payload[3] = 0x00;                                 // Reserved
        mcm_module->u8_send_payload[4] = sizeof(ver_type_1_t);                 // Payload length
        mcm_module->u8_send_payload[5] = version.major;                      
        mcm_module->u8_send_payload[6] = version.minor;
        mcm_module->u8_send_payload[7] = version.patch;

        // Append CRC
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append CRC\n");
            break;
        }

        // Send the payload
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(
            mcm_module->u8_send_payload, max_payload_size, mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }

        return_status = API_PROCESSOR_SUCCESS;


    } while (0);

    return return_status;
}

api_processor_status_t api_processor_cmd_get_seg_file_transfer_status(mcm_module_hdl_t *mcm_module)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        // Clear the payload buffer
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN;

        // Construct the payload
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_GENERAL;                  // Command type
        mcm_module->u8_send_payload[1] = MROVER_CC_FILE_STATUS >> 8;     // Command code MSB
        mcm_module->u8_send_payload[2] = MROVER_CC_FILE_STATUS & 0xFF;   // Command code LSB
        mcm_module->u8_send_payload[3] = 0x00;                                 // Reserved
        mcm_module->u8_send_payload[4] = 0x00;                 // Payload length
        mcm_module->u8_send_payload[5] = 0x00;                      

        // Append CRC
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append CRC\n");
            break;
        }

        // Send the payload
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(
            mcm_module->u8_send_payload, max_payload_size, mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }

        return_status = API_PROCESSOR_SUCCESS;


    } while (0);

    return return_status;

}

api_processor_status_t api_processor_cmd_trigger_fw_update(mcm_module_hdl_t *mcm_module, ver_type_1_t version)
{
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    do
    {
        if (NULL == mcm_module || NULL == mcm_module->h_serial_device.send_data_cb)
        {
            TRACE_INFO("MCM module is not initialized\n");
            break;
        }

        // Clear the payload buffer
        memset(mcm_module->u8_send_payload, 0, MAX_SERIAL_SEND_PAYLOAD_SIZE);
        const uint16_t max_payload_size = MIN_TX_PAYLOAD_LEN + sizeof(ver_type_1_t);

        // Construct the payload
        mcm_module->u8_send_payload[0] = COMMAND_TYPE_GENERAL;                  // Command type
        mcm_module->u8_send_payload[1] = MROVER_CC_TRIGGER_FW_UPDATE >> 8;     // Command code MSB
        mcm_module->u8_send_payload[2] = MROVER_CC_TRIGGER_FW_UPDATE & 0xFF;   // Command code LSB
        mcm_module->u8_send_payload[3] = 0x00;                                 // Reserved
        mcm_module->u8_send_payload[4] = sizeof(ver_type_1_t);                 // Payload length
        mcm_module->u8_send_payload[5] = version.major;                      
        mcm_module->u8_send_payload[6] = version.minor;
        mcm_module->u8_send_payload[7] = version.patch;                    

        // Append CRC
        fp_api_status_t status = fp_append_crc(mcm_module->u8_send_payload, max_payload_size);
        if (FP_SUCCESS != status)
        {
            TRACE_INFO("Failed to append CRC\n");
            break;
        }

        // Send the payload
        uint16_t u16_sent_bytes = mcm_module->h_serial_device.send_data_cb(
            mcm_module->u8_send_payload, max_payload_size, mcm_module->user_context);
        if (max_payload_size != u16_sent_bytes)
        {
            TRACE_INFO("Failed to send data through serial port\n");
            return_status = API_PROCESSOR_SERIAL_PORT_ERROR;
            break;
        }

        return_status = API_PROCESSOR_SUCCESS;


    } while (0);

    return return_status;

}

/**
 * @brief This function handles the uplink request from the MCM module.
 *
 * @param[in] mcm_module Pointer to the MCM module structure.
 * @param[in] data Pointer to the data buffer containing the uplink request.
 * @param[in] len Length of the data buffer containing the uplink request.
 * @param[out] p_response Pointer to the api_processor_response_t structure
 *                       that needs to be filled up.
 *
 * @return API_PROCESSOR_SUCCESS if the request is processed successfully,
 *         otherwise appropriate error code.
 */
static api_processor_status_t api_processor_handle_request_uplink(mcm_module_hdl_t *mcm_module, uint8_t *data, uint16_t len, api_processor_response_t *p_response) {
    api_processor_status_t return_status = API_PROCESSOR_ERROR;

    // Basic sanity checks
    if (data == NULL || len < 1) {
        TRACE_INFO("Invalid data for uplink request\n");
        return API_PROCESSOR_INVALID_PARAMETERS;
    }

    // Debug print to indicate the start of processing the uplink request
    TRACE_INFO("Processing uplink request with length: %d\n", len);

    // @todo : Noman - Process the uplink request here

    // Assuming processing is successful
    return_status = API_PROCESSOR_SUCCESS;

    return return_status;
}







