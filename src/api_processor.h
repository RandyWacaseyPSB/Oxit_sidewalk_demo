/**
 * @file api_processor.h
 * @author Ankit Bansal (ankit.bansal@oxit.com)
 * @brief This file contains the header for the api used by the application. 
 * @version 0.1
 * @date 2024-04-11
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

#ifndef __API_PROCESSOR_H__
#define __API_PROCESSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
 * INCLUDES
 **********************************************************************************************************/
#include <stdint.h>
#include "commands_defs.h"


/**********************************************************************************************************
 * MACROS AND DEFINES
 **********************************************************************************************************/

/**
 * @brief Major, minor, and patch version numbers for the API Processor module.
 * 
 * @note These values are used to track the version of the API Processor module.
 *       The major version number is incremented for incompatible changes,
 *       the minor version number is incremented for compatible changes, and
 *       the patch version number is incremented for bug fixes.
 */
#define API_PROCESSOR_LIB_MAJOR_VERSION 0
#define API_PROCESSOR_LIB_MINOR_VERSION 2
#define API_PROCESSOR_LIB_PATCH_VERSION 0

/**********************************************************************************************************
 * TYPEDEFS
 **********************************************************************************************************/

typedef enum {
    API_PROCESSOR_SUCCESS = 0,              // command executed with not error
    API_PROCESSOR_INVALID_PARAMETERS,       // the parameters are invalid for the command
    API_PROCESSOR_SERIAL_PORT_ERROR,       // error in the serial port  
    API_PROCESSOR_INVALID_SERIAL_DATA,      // serial data in not valid. Most probably due to length or null data
    API_PROCESSOR_ERROR                     // other error associated with the command.
}api_processor_status_t;


/**
 * @brief This the two bytes of the reset count.
 * This is the data for the event `GET_EVENT_RESET`
 * 
 */
typedef struct
{
    uint16_t reset_count;
} get_event_reset_t; 

/**
 * @brief Structure to get the tx status. 
 * This is the data for the event 'GET_EVENT_TX_STATUS'
 * 
 */
typedef struct
{
    mrover_uplink_event_type_t tx_status;
} get_tx_status_t;




/**
 * @brief Downlink data received from the server
 * 
 * This is the data for the event 'GET_EVENT_DOWN_DATA'.
 * This structure contains the downlink data received from the server.
 * The structure also contains the RSSI (Received Signal Strength Indicator)
 * and SNR (Signal to Noise Ratio) values for the received data.
 * The lrwan_sid_seq_port is used to store either the LoRaWAN downlink port
 * or the Sequence Number used in Sidewalk communication.
 * 
 */
typedef struct
{
    int8_t rssi;
    int8_t snr;
    uint8_t *payload;
    uint16_t payload_len;
    uint16_t lrwan_sid_seq_port;
} get_evt_down_data_t;

typedef struct
{
    mrover_lorawan_class_t new_class;
}get_evt_class_switch_data_t;

typedef struct __attribute__((packed))
{

    uint8_t bin_type:4;//lower 4 bits
    uint8_t cmd_type:4;//higher 4 bits
} cmd_type_t;

/**
 * @brief versioning scheme
 *  1 byte major
 *  1 byte minor 
 *  1 byte minor
 *  This structure is used for GET_VERSION command response.or
 *  version of mcm rover library
 */
typedef struct 
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
}ver_type_1_t;

typedef struct __attribute__((packed))
{   
    cmd_type_t cmd_type;
    ver_type_1_t fw_ver;
    uint8_t pkg_size[3];
    uint8_t seg_size:4; // lower nibble
    uint8_t nxt_seg_id:4; // higher nibble
    uint16_t seg_status;
}get_seg_file_status_t;

/**
 * @brief Union to hold the event data which is the response of the command
 *         based on the event received.
 *         This union is used for the event response. The data will be filled
 *         based on the event received.
 *         For example, if the event is GET_EVENT_RESET, the reset_data in union
 *         will be filled. If the event is GET_EVENT_DOWN_DATA, the down_data in union
 *         will be filled, etc.
 *
 */
typedef union
{
    get_event_reset_t reset_data;
    get_tx_status_t tx_status_data;
    get_evt_down_data_t down_data;
    get_evt_class_switch_data_t class_switch_data;
    get_seg_file_status_t download_segment_data;
} get_event_cb_value_t;


/**
 * @brief Structure to hold the event data which is the response of the command
 *        GET_EVENT.
 *
 * This structure contains the event code and data associated with that event.
 * The data type of the data will depend on the event code.
 * Refer to the get_event_code_t enum for the data associated with each event.
 */
typedef struct
{
    get_event_code_t get_event_code;
    get_event_cb_value_t get_event_data_value;
} get_event_data_t;





/**
 * @brief Structure to hold the version of each component which is
 *        having the 1 byte major, 1 byte minor, and 2byte  patch version.
 *        The structure is used for GET_VERSION command response.
 *
 */
typedef struct 
{
    uint8_t major;
    uint8_t minor;
    uint16_t patch;
    }ver_type_2_t;

/**
 * @brief Structure to hold the version of each component 
 *        Oxit mcm guide 4.5.3.2
 *
 * This structure contains the version of each component of the mcm module.
 * The version number is in the format major.minor.patch.
 *
 */

typedef struct 
{
    ver_type_2_t bootloader;
    ver_type_2_t modem_fw;
    ver_type_1_t modem_hw;
    ver_type_1_t sidewalk;
    ver_type_1_t lorawan;
}get_ver_data_t;




/**
 * @brief Structure to hold the command response data
 *
 * Since now we do not have any response data for any other command.
 * But in future if we need to add response data for any command then we can
 * add the response data in this structure
 *
 */
typedef union
{
    get_event_data_t get_event_data;
    get_ver_data_t ver_info;
    const uint8_t* dev_eui;
    const uint8_t* join_eui;
    get_seg_file_status_t seg_file_status;
} cmd_response_data_t;

/**
 * @brief This the response breakdown of the response received on the uart.
 * This structure has been parse with the related values. 
 * user can use this stucture to get the response of any command
 * Alternatively, helper functions have been provided 
 * to get the response for the selective commands
 * It is recommended to use the helper functions, rather than 
 * the bare structure, if helper functions have been implemented for that particular command
 * 
 */
typedef struct
{   
    mrover_return_code_t return_code;       // return code for the command. MROVER_RC_OK if success
    command_types_t cmd_type;               // command type, general,lorawan, sidewalk
    mrover_cc_codes_t cmd_code;             // command code
    cmd_response_data_t cmd_response_data;  // command response data received
} api_processor_response_t;

typedef uint16_t (*serial_send_data_cb)(uint8_t *data, uint16_t size,void* user_context);
typedef void (*serial_receive_data_cb)(uint8_t *data,uint16_t size,void* user_context);
typedef void (*mrover_notification_cb)(void *user_context);
typedef void(*mrover_response_cb)(const api_processor_response_t *response,void* user_context);

typedef struct
{
    serial_send_data_cb send_data_cb;
} serial_module_hdl_t;  


typedef struct {
    serial_module_hdl_t h_serial_device;
    mrover_notification_cb handle_notification_cb;              // callback function for notification 
    mrover_response_cb handle_response_cb;                        // callback for response 
    uint8_t u8_send_payload[MAX_SERIAL_SEND_PAYLOAD_SIZE];
    uint8_t u8_received_payload[MAX_SERIAL_RECEIVE_PAYLOAD_SIZE];
    uint8_t _no_of_curr_pen_evt;                             // keep the context for number of current pending events, private variable need not to be access directly
    void *user_context;
}mcm_module_hdl_t; 

/**********************************************************************************************************
 * EXPORTED VARIABLES
 **********************************************************************************************************/

/**********************************************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 **********************************************************************************************************/

/**
 * @brief Initializes the API processor module
 *
 * @param[in,out] mcm_module Pointer to the MCM module handle
 * @param[in] send_data_cb Callback function for sending data
 * @param[in] h_mrover_notification_cb Callback function for notifications
 * @param[in] h_mrover_response_cb Callback function for responses
 * @return  API_PROCESSOR_SUCCESS if data is parsed successfully otherwise error code
 */
api_processor_status_t api_processor_init(
                        mcm_module_hdl_t *mcm_module,
                        serial_send_data_cb send_data_cb,
                        mrover_notification_cb h_mrover_notification_cb,
                        mrover_response_cb h_mrover_response_cb
);

/**
 * @brief Retrieves the library version
 *
 * @param[out] ver Pointer to a version structure to store the library version
 * @return void
 */
void api_processor_get_lib_ver(ver_type_1_t *ver);

/**
 * @brief Parses the received serial data and processes it
 *
 * @param[in] mcm_module Pointer to the MCM module handle
 * @param[in] data Pointer to the received data
 * @param[in] len Length of the received data
 * @return API_PROCESSOR_SUCCESS if data is parsed successfully otherwise error code
 */
api_processor_status_t api_processor_parse_rx_data(mcm_module_hdl_t *mcm_module,uint8_t* data,uint16_t len);

/**
 * @brief Returns the number of pending events reported by mcm module
 *
 * @param [in] mcm_module Pointer to the MCM module handle
 * @return uint8_t Number of pending events
 */
uint8_t api_processor_get_pending_events(mcm_module_hdl_t *mcm_module);

/****************Transmitting functions Prototypes**************/

/**
 * @brief This command can be used to retrieve pending events from the Modem.
 *
 * @note
 * 1. The application has a queue that can store events, maximum 10 events, in a FIFO
 *    (First-In, First-Out) manner. When the queue is full, any new events will cause
 *    the oldest event (the first element) to be deleted to make room for the new event.
 *
 * @note 2. On boot up, the modem will trigger a reset event, either by using the
 *    NotifyEvent command or the EVENT line.
 *
 * @note 3. After sending the Reset/Factory Reset command, the application must
 *    retrieve the reset event before the modem proceeds with reset.
 *
 * @note 4. When no event is available, this command returns with an empty response
 *    payload.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device
 *                      callback functions.
 *
 * @return Returns the status of the API processor initialization.
 */
api_processor_status_t api_processor_cmd_get_event(mcm_module_hdl_t *mcm_module);

/**
 * @brief This command retrieves information regarding the bootloader version, installed application
        firmware version, hardware version, Sidewalk stack version and LoRaWAN version.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device callback
 *                      functions.
 *
 * @return Returns the status of the API processor initialization.
 */
api_processor_status_t api_processor_cmd_get_version(mcm_module_hdl_t *mcm_module);

/**
 * @brief This command performs a reset of the Modem MCU. All transient state information
 *        (including session data) will be lost. The Application MCU must retrieve the
 *        reset event by sending the GetEvent command so the modem can proceed with reset.
 *        The modem will reset after 500ms from retrieving the reset event.
 *
 * @note 1. If Sidewalk protocol (any link type) is initialized, then the reset command
 *        will stop the protocol and the user will need to initialize the required sidewalk
 *        link type again.
 *
 * @note 2. If LoRaWAN protocol is initialized and joined on the network, EUIs and Keys
 *        will be lost upon RESET command and the user will need to initialize, configure
 *        and join again.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device callback
 *                      functions.
 *
 * @return Returns the status of the API processor initialization.
 */
api_processor_status_t api_processor_cmd_reset(mcm_module_hdl_t *mcm_module);

/**
 * @brief This function sends the factory reset command to the
 *        serial interface.
 *
 * @note 1. Factory reset command won’t erase the reset counter.
 * @note 2. Factory reset command does not erase the sidewalk
 *        manufacturing file.
 * @note 3. If Sidewalk protocol (any link type) is initialized and
 *        registered on server, then factory reset command will
 *        deregister module from sidewalk network.
 * @note 4. If LoRaWAN protocol is initialized and joined on the
 *        network, EUIs and Keys will be erased and devNonce
 *        will be retained.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the
 *            serial device callback functions.
 *
 * @return Returns the status of the API processor initialization.
 */
api_processor_status_t api_processor_cmd_factory_reset(mcm_module_hdl_t *mcm_module);

/**
 * @brief   This command performs a factory reset of the Modem MCU. All transient state information
            (including session data) will be lost and the Modem will need to join the sidewalk network again.
 *          The Application MCU must retrieve the reset event by sending the GetEvent so the modem can
            proceed with reset. The modem will factory reset after 500ms from retrieving the reset event.
 * @note    1. This command will be functional only when either of the protocols is in operation.
 *          2. When switching to Sidewalk, the FSK Link type is automatically selected by default.
 *          3. If LoRaWAN was joined on the network after configuring EUIs and Keys, switching
            protocols will result in the loss of these configurations. Users will need to reconfigure
            them.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device callback
 *                      functions.
 *
 * @return Returns the status of the command send operation.
 */
api_processor_status_t api_processor_cmd_switch_network(mcm_module_hdl_t *mcm_module);

/**
 * @brief This function sends the init lorawan command to
 * the serial interface.
 *
 * @note 1.This command will be functional only when another protocol (Sidewalk) is not running
        and LoRaWAN is in stop condition. If given otherwise this command will do nothing.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the
 *            serial device callback functions.
 *
 * @return Returns the status of the command send operation.
 */
api_processor_status_t api_processor_cmd_init_lorawan(mcm_module_hdl_t *mcm_module);

/**
 * @brief This command is used to configure the Join EUI for LoRaWAN.
 *        User needs to enter 8 bytes of desired Join EUI in the payload
 *        of the input command.
 *
 * @note 1. This command will be functional only when LoRaWAN protocol is
 *        initialized. If given otherwise this command will do nothing.
 * @note 2. The command can only be issued as long as the Modem has not yet
 *        joined, or is trying to join, the network. Otherwise, the command
 *        will fail.
 *
 * @param mcm_module is a pointer to the MCM module.
 * @param p_join_eui is a pointer to the join EUI.
 * @param u8_join_eui_len is the length of the join EUI.
 *
 * @return API_PROCESSOR_SUCCESS if the command is executed successfully,
 *         else an error code.
 */
api_processor_status_t api_processor_cmd_set_join_eui(mcm_module_hdl_t *mcm_module, uint8_t *p_join_eui,uint8_t u8_join_eui_len);

/**
 * @brief This command is used to configure the Device EUI for LoRaWAN.
 *        User needs to enter 8 bytes of desired Dev EUI in the payload
 *        of the input command.
 *
 * @note 1. This command will be functional only when LoRaWAN protocol is
 *        initialized. If given otherwise this command will do nothing.
 * @note 2. The command can only be issued as long as the Modem has not yet
 *        joined, or is trying to join, the network. Otherwise, the command
 *        will fail.
 *
 * @param mcm_module is a pointer to the MCM module.
 * @param p_dev_eui is a pointer to the device EUI.
 * @param u8_dev_eui_len is the length of the device EUI.
 *
 * @return API_PROCESSOR_SUCCESS if the command is executed successfully,
 *         else an error code.
 */
api_processor_status_t api_processor_cmd_set_dev_eui(mcm_module_hdl_t *mcm_module, uint8_t *p_dev_eui,uint8_t u8_dev_eui_len);

/**
 * @brief This function sets the network key to be used with the LoRaWAN network.
 *
 * @note 1. This command will be functional only when LoRaWAN protocol is initialized.
 *         If given otherwise this command will do nothing.
 * @note 2. The command can only be issued as long as the Modem has not yet joined,
 *         or is trying to join, the network. Otherwise, the command will fail.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device
 *                       callback functions.
 * @param[in] p_nwk_key Pointer to the network key to be set.
 *
 * @return Returns the status of the command send operation.
 */
api_processor_status_t api_processor_cmd_set_nwk_key(mcm_module_hdl_t *mcm_module, uint8_t *p_nwk_key, uint16_t u16_nwk_key_len);

/**
 * @brief This function sends the command to retrieve the configured Device EUI for LoRaWAN.
 *        This command will return '8 bytes Dev EUI' in response command.
 *
 * @note 1. This command will be functional only when LoRaWAN protocol is initialized.
 *         If given otherwise this command will do nothing.
 * @note 2. This command will return '0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00', 8 Bytes of Dev
 *         EUI if Dev EUI is not yet configured by the user.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device callback
 *                      functions.
 *
 * @return Returns the status of the API processor.
 */
api_processor_status_t api_processor_cmd_get_dev_eui(mcm_module_hdl_t *mcm_module);


/**
 * @brief This command is used to get the Join EUI for LoRaWAN.
 *
 * @note 1. This command will be functional only when LoRaWAN protocol is initialized.
 *         If given otherwise, this command will do nothing.
 * @note 2. This command will return '0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00',
 *         8 Bytes of Join EUI if Join EUI is not yet configured by the user.
 *
 * @param mcm_module is a pointer to the MCM module.
 * @return API_PROCESSOR_SUCCESS if the command is executed successfully, else an error code.
 */
api_processor_status_t api_processor_cmd_get_join_eui(mcm_module_hdl_t *mcm_module);


/**
 * @brief This command is used to join the LoRaWAN network.
 *
 * @note 1. This command will be functional only when LoRaWAN protocol is initialized.
 *         If given otherwise, this command will do nothing.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device
 *                       callback functions.
 *
 * @return API_PROCESSOR_SUCCESS if the command is executed successfully, else an error code.
 */
api_processor_status_t api_processor_cmd_join_lorawan(mcm_module_hdl_t *mcm_module);

/**
 * @brief 'Request Uplink' command requests sending the given data on the specified port as an
 *        unconfirmed or confirmed frame. The payload of the request uplink specifies the confirmed or
 *        unconfirmed frame as below:
 *        - MROVER_UPLINK_UNCONFIRMED: unconfirmed data
 *        - MROVER_UPLINK_CONFIRMED: confirmed data
 *
 * @note 1. The application shall not use port 0 or the ports from 225 to 255 since they are reserved
 *         for future standardized application extensions.
 *
 * @param[in] mcm_module MCM module handle
 * @param[in] u8_port Port number on which data needs to be sent
 * @param[in] u8_payload Pointer to the payload to be sent
 * @param[in] u16_payload_size Length of the payload to be sent
 * @param[in] h_uplink_type Type of the uplink frame:
 *                          - unconfirmed or confirmed
 * @return API_PROCESSOR_SUCCESS on success, API_PROCESSOR_ERROR on error.
 */
api_processor_status_t api_processor_cmd_request_lorawan_uplink(mcm_module_hdl_t *mcm_module,uint8_t u8_port,uint8_t *u8_payload,uint16_t u16_payload_size,mrover_uplink_type_t h_uplink_type);


/**
 * @brief This command is used to leave the LoRaWAN network.
 *
 * @note This command will be functional only when LoRaWAN protocol is initialized.
 *       If given otherwise, this command will do nothing.
 *
 * @param[in] mcm_module 
 *           is a pointer to the MCM module (input).
 *
 * @return 
 *         API_PROCESSOR_SUCCESS if the command is executed successfully,
 *         else an error code.
 */
api_processor_status_t api_processor_cmd_leave_lorawan_network(mcm_module_hdl_t* mcm_module);

/**
 * @brief Stop the LoRaWAN network
 *
 * @note 1. This command will be functional only when LoRaWAN protocol is initialized.
 *         If given otherwise, this command will do nothing.
 *
 * @note 2. If LoRaWAN protocol is initialized and joined on the network, the EUIs and Keys
 *         will be lost upon Stop command, and the user will need to initialize, configure,
 *         and join again.
 *
 * @note 3. All commands related to the LoRaWAN protocol will fail until LoRaWAN is initialized
 *         again.
 *
 * @note 4. No messages will be sent or received via LoRaWAN protocol after issuing LoRaWAN
 *         stop command.
 *
 * @param[in] mcm_module : MCM module handle
 *
 * @return API_PROCESSOR_SUCCESS if success, otherwise error code
 */
api_processor_status_t api_processor_cmd_stop_lorawan_network(mcm_module_hdl_t* mcm_module);

/**
 * @brief BLE Link Request command will initialize the BLE library and start
 *        the BLE link.
 *
 * @note 1. This command will be functional only when another Sidewalk
 *          protocol (FSK/CSS) is not running and LoRaWAN is in stop
 *          condition. If given otherwise, this command will do nothing.
 *
 * @note 2. If the device is not registered, the modem will not be able to
 *          ever sync with GW. To make sure the device is registered, a BLE
 *          link request should be initiated first as the registration
 *          happens through BLE/FSK. After receiving the time sync event
 *          through BLE, users can switch between different link types and
 *          start communicating with the Sidewalk network.
 *
 * @note 3. FSK/CSS and BLE commands initiate requests to join the Sidewalk
 *          network. During the procedure, the modem will try to time sync
 *          with the gateway, a time sync failure or time sync success events
 *          will be generated to indicate if the modem successfully
 *          synchronized its time with the network or not.
 *
 * @param[in] mcm_module pointer to the mcm module handle
 *
 * @return API_PROCESSOR_SUCCESS if command was sent successfully, else error
 *         code
 */
api_processor_status_t api_processor_cmd_sid_ble_link_request(mcm_module_hdl_t* mcm_module);


/**
 * @brief This function sends the BLE connection request command to the MCM
 *        module.
 *
 * @note 1. This command will be functional only when ‘BLE Link Request’ is
 *         established already, if given otherwise this command will do
 *         nothing.
 *
 * @note 2. According to the Sidewalk protocol specification, the BLE
 *         connection will timeout after 30 seconds and disconnect itself, if
 *         no uplink or downlink is detected within the 30 seconds.
 *
 * @note 3. Sending a downlink when Sidewalk is connected through BLE, will
 *         auto initiate a connection with GW as per Sidewalk
 *         specification and downlink will be received, no need to send BLE
 *         Connection request for downlinks.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device
 *                      callback functions.
 *
 * @return Returns the status of the API processor initialization.
 */
api_processor_status_t api_processor_cmd_sid_ble_conn_request(mcm_module_hdl_t* mcm_module);

/**
 * @brief This function sends the FSK connection request command to the Sidewalk
 *        module.
 *
 * @note 1. This command will be functional only when another Sidewalk protocol
 *         (BLE/CSS) is not running and LoRaWAN is in stop condition. If given
 *         otherwise, this command will do nothing.
 *
 * @note 2. FSK Link Request commands initiate requests to join the Sidewalk
 *         network. During the procedure, the modem will try to time sync with
 *         the gateway, a time sync failure or time sync success events will be
 *         generated to indicate if the modem successfully synchronized its time
 *         with the network or not.
 *
 * @param[in] mcm_module Pointer to the MCM module structure which has the serial
 *                      device callback functions.
 *
 * @return Returns the status of the FSK connection request command.
 */
api_processor_status_t api_processor_cmd_sid_fsk_link_request(mcm_module_hdl_t* mcm_module);

/**
 * @brief This function sends the sidewalk CSS link request command over the serial
 *        port.
 *
 * @note 1. This command will be functional only when another Sidewalk protocol
 *          (BLE/FSK) is not running and LoRaWAN is in stop condition. If given
 *          otherwise, this command will do nothing.
 *
 * @note 2. When this command is issued for the first time after power cycling the
 *          device, it will automatically initialize and start CSS power profile
 *          B. The user can use the Set CSS Profile command to switch to power
 *          profile A.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device
 *                      callback functions.
 *
 * @return Returns the status of the API processor after sending the command.
 */
api_processor_status_t api_processor_cmd_sid_css_link_request(mcm_module_hdl_t* mcm_module);

/**
 * @brief This function sends the CSS profile to the mcm sidewalk module.
 *
 * @note 1. This command will only be functional if the CSS Sidewalk protocol
 *        (BLE/FSK) is running. If given otherwise, this command will do
 *        nothing.
 * @note 2. For CSS Profile A, the rx window count is 5.
 * @note 3. Default CSS power profile is B.
 *
 * @param[in] mcm_module Pointer to the MCM module structure
 * @param[in] h_profile The profile to be set
 *
 * @return API_PROCESSOR_SUCCESS if the command is executed successfully,
 *         otherwise an error code is returned.
 *
 */
api_processor_status_t api_processor_cmd_sid_set_css_profile(mcm_module_hdl_t* mcm_module,mrover_css_pwr_profile_t h_profile);

/**
 * @brief This function sends a command to send uplink data to Sidewalk Network.
 *
 * The request will be queued and the frame will be sent as soon as bandwidth
 * is available.
 *
 * @note 1. Maximum transmission for different link types:
 *       - BLE: 255 bytes
 *       - FSK: 200 bytes
 *       - CSS: 19 bytes
 *
 * @note 2. The modem should be time synced with GW to be able to transmit/receive
 *       data.
 *
 * @note 3. For BLE, the user should initiate a Connection request using the BLE
 *       Connection Request command to be able to transmit data.
 *
 * @note 4. Cannot send empty uplink.
 *
 * @param[in] mcm_module       pointer to the mcm_module_hdl_t struct.
 * @param[in] u8_payload       pointer to the payload to be sent.
 * @param[in] u16_payload_size size of the payload.
 * @param[in] h_uplink_type    Type of the uplink frame: unconfirmed or confirmed
 *
 * @return API_PROCESSOR_SUCCESS if the data is sent successfully,
 *         API_PROCESSOR_ERROR if failed to send the data,
 *         API_PROCESSOR_INVALID_PARAMETERS if the parameters are invalid.
 */
api_processor_status_t api_processor_cmd_sid_send_uplink(mcm_module_hdl_t* mcm_module, uint8_t *u8_payload, uint16_t u16_payload_size, mrover_uplink_type_t h_uplink_type);

/**
 * @brief Set the set the received duplicated messages policy filter
 * 
 * @param[in] mcm_module Pointer to the mcm_module_hdl_t instance
 * @param[in] h_filtering Filtering option: enable or disable
 * @return API_PROCESSOR_SUCCESS if successful, otherwise an error code
 */
api_processor_status_t api_processor_cmd_set_sid_downlink_filter(mcm_module_hdl_t* mcm_module, mrover_sid_downlink_filter_t h_filtering);

/**
 * @brief Stops an already established sidewalk link (BLE/FSK/CSS)
 *
 * @note 1. This command will only be functional when Sidewalk (any of the links
 *         BLE/FSK/CSS) protocol is initialized. If given otherwise, this command
 *         will do nothing.
 *
 * @note 2. If Sidewalk protocol is initialized and registered on the network,
 *         stopping the sidewalk protocol will NOT deregister the device from the
 *         network.
 *
 * @note 3. All commands related to Sidewalk protocol will fail until Sidewalk
 *         (BLE/FSK/CSS) is initialized again.
 *
 * @note 4. No messages will be sent or received via Sidewalk protocol after
 *         issuing sidewalk stop command.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device
 *                      callback functions.
 *
 * @return Returns the status of the API processor function.
 */
api_processor_status_t api_processor_cmd_sid_stop(mcm_module_hdl_t* mcm_module);

/**
 * @brief This command is used to set the LoRaWAN device class (A, B, or C).
 *
 * @note 1. This command will be functional only when LoRaWAN protocol is initialized.
 *          If given otherwise, this command will do nothing.
 * @note 2. The command can only be issued when the device is not currently joined
 *          to a network.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device
 *                       callback functions.
 * @param[in] lorawan_class The desired LoRaWAN class (A, B, or C)
 *
 * @return API_PROCESSOR_SUCCESS if the command is executed successfully,
 *         else an error code.
 */
api_processor_status_t api_processor_cmd_set_lorawan_class(mcm_module_hdl_t *mcm_module, 
                                                         mrover_lorawan_class_t lorawan_class);

/**
 * @brief This command retrieves the current LoRaWAN device class configuration.
 *
 * @note 1. This command will be functional only when LoRaWAN protocol is initialized.
 *          If given otherwise, this command will do nothing.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device
 *                       callback functions.
 *
 * @return API_PROCESSOR_SUCCESS if the command is executed successfully,
 *         else an error code.
 */
api_processor_status_t api_processor_cmd_get_lorawan_class(mcm_module_hdl_t *mcm_module);

/**
 * @brief This command initiates the file transfer process.
 *
 * @note 1. This command will be functional only when the module is properly initialized.
 *          If given otherwise, this command will do nothing.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device
 *                       callback functions.
 * @param[in] version The version information required for the file transfer.
 *
 * @return API_PROCESSOR_SUCCESS if the command is executed successfully,
 *         else an error code.
 */
api_processor_status_t api_processor_cmd_start_file_transfer(mcm_module_hdl_t *mcm_module, ver_type_1_t version);

/**
 * @brief This command retrieves the status of the segmented file transfer.
 * 
 */
api_processor_status_t api_processor_cmd_get_seg_file_transfer_status(mcm_module_hdl_t *mcm_module);

/**
 * @brief This command triggers a firmware update process.
 *
 * @note 1. This command will be functional only when the module is properly initialized.
 *          If given otherwise, this command will do nothing.
 *
 * @param[in] mcm_module Pointer to the MCM module which has the serial device
 *                       callback functions.
 * @param[in] version The version information required for the firmware update.
 *
 * @return API_PROCESSOR_SUCCESS if the command is executed successfully,
 *         else an error code.
 */
api_processor_status_t api_processor_cmd_trigger_fw_update(mcm_module_hdl_t *mcm_module, ver_type_1_t version);



/***********************************Helper Functions Prototypes *********************************/


/**
 * @brief Get the response code from the API processor response
 * 
 * @param[in] res Pointer to the API processor response
 * @return The response code
 */
mrover_return_code_t mcm_helper_get_response_code(const api_processor_response_t *res);

/**
 * @brief Get the command type from the API processor response
 * 
 * @param[in] res Pointer to the API processor response
 * @return The command type, command_types_t
 */
command_types_t mcm_helper_get_command_type(const api_processor_response_t *res);

/**
 * @brief Get the command code from the API processor response
 * 
 * @param[in] res Pointer to the API processor response
 * @return The command code, mrover_cc_codes_t
 */
mrover_cc_codes_t mcm_helper_get_command_code(const api_processor_response_t *res);

/**
 * @brief Get the event code from the API processor response
 * 
 * @param[in] res Pointer to the API processor response
 * @return The event code get_event_code_t
 */
get_event_code_t mcm_helper_get_event_code(const api_processor_response_t *res);

/**
 * @brief Get the event TX status from the API processor response
 * 
 * @param[in] res Pointer to the API processor response
 * @return The event TX status mrover_uplink_event_type_t
 */
mrover_uplink_event_type_t mcm_helper_get_event_tx_status(const api_processor_response_t *res);

/**
 * @brief Get the device EUI from the API processor response
 * 
 * @param[in] res Pointer to the API processor response
 * @return The pointer to the device EUI
 */
const uint8_t* mcm_helper_get_dev_eui(const api_processor_response_t *res);

/**
 * @brief Get the join EUI from the API processor response
 * 
 * @param[in] res Pointer to the API processor response
 * @return The pointer to the join EUI
 */
const uint8_t* mcm_helper_get_join_eui(const api_processor_response_t *res);

/**
 * @brief Get the versions from the API processor response
 * 
 * @param[in] res Pointer to the API processor response
 * @param[out] bootloader Pointer to the bootloader version
 * @param[out] mdm_fw Pointer to the MDM FW version
 * @param[out] mdm_hw Pointer to the MDM HW version
 * @param[out] sidwlk Pointer to the Sidewalk stack version
 * @param[out] lrwan Pointer to the LoRaWAN stack version
 */
void mcm_helper_get_version(const api_processor_response_t *res, ver_type_2_t *bootloader, ver_type_2_t *mdm_fw,
                            ver_type_1_t *mdm_hw, ver_type_1_t *sidwlk, ver_type_1_t *lrwan);

/**
 * @brief Get the length of the downlink data from the API processor response
 * 
 * @param[in] res Pointer to the API processor response
 * @return The length of the downlink data
 */
uint16_t mcm_helper_get_downlink_len(const api_processor_response_t *res);

/**
 * @brief Get the downlink data from the API processor response
 * 
 * @param[in] res Pointer to the API processor response
 * @param[out] rssi Pointer to the RSSI value
 * @param[out] snr Pointer to the SNR value
 * @param[out] payload Pointer to the payload
 * @param[out] port_seq Pointer to the port and sequence number
 */
void mcm_helper_get_downlink_data(const api_processor_response_t *res, int8_t *rssi, int8_t *snr, uint8_t *payload, uint16_t *port_seq);

/**
 * @brief Get the reset count from the API processor response
 * 
 * @param[in] res Pointer to the API processor response
 * @return The reset count
 */
uint8_t mcm_helper_get_event_reset_count(const api_processor_response_t *res);

mrover_lorawan_class_t mcm_helper_get_event_new_class(const api_processor_response_t *res);

get_seg_file_status_t mcm_helper_get_event_seg_down(const api_processor_response_t *res);


mrover_lorawan_class_t mcm_helper_get_device_class(const api_processor_response_t *res);

void mcm_helper_get_seg_file_status(const api_processor_response_t *res, get_seg_file_status_t *seg_file_status);

#ifdef __cplusplus
}
#endif
#endif // __API_PROCESSOR_H__