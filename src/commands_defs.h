/**
 * @file commands.h
 * @author Ankit Bansal (ankit.bansal@oxit.com)
 * @brief This describes the command codes used by the mcm module.
 * @version 0.1
 * @date 2024-04-10
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


#ifndef __COMMANDS_DEFS_H__
#define __COMMANDS_DEFS_H__


#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
 * INCLUDES
 **********************************************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/**********************************************************************************************************
 * MACROS AND DEFINES
 **********************************************************************************************************/
/**
 * Oxtech mcm user guide 4.4.1
*/
#define MAX_SERIAL_SEND_PAYLOAD_SIZE                306

/**
 * @brief Assume cmd type =1 cmd code = 2 len =2 payload = 0 checksum = 1 
 * 
 */
#define MIN_TX_PAYLOAD_LEN                           6 
/**
 * @brief in case of notify events  
 *  1 byte cmd code , 2 byte len, 1 byte payload and 1 byte crc
 */
#define MIN_RX_PAYLOAD_LEN                          5

#define LENGTH_IN_NOTIFICATION_PAYLOAD               0x0001

#define MAX_PENDING_MESSAGES                        0x0A
/**
 * Oxtech mcm user guide 4.4.2
*/
#define MAX_SERIAL_RECEIVE_PAYLOAD_SIZE             307

/**
 * @brief oxtech guide 4.5.3.2
 * 
 */
#define GET_VERSION_RESPONSE_PAYLOAD_LEN             17

/**
 * @brief In case of get event downlink data 
 *  1 byte for the snr
 *  1 byte for the rssi
 *  1 byte for the port,
 *  atleast would be there 
 *  
 */
#define MIN_DOWNLINK_PAYLOAD_LEN                    3

#define LORAWAN_DEV_EUI_JOIN_EUI_LEN                8

#define LORAWAN_NETWORK_KEY_LEN                     16

/**
 * @brief Maximum size of the user data in lorawan tx
 * 
 */
#define LORAWAN_TX_MAX_PAYLOAD_SIZE                 350

#define SIDEWALK_TX_MAX_BLE_PAYLOAD_SIZE            255
#define SIDEWALK_TX_MAX_FSK_PAYLOAD_SIZE            200
#define SIDEWALK_TX_MAX_CSS_PAYLOAD_SIZE            19

/**
 * @brief 4.5.3.22 Set Filtering Option For Sidewalk Downlink
 * 
 */
#define SIDEWALK_ENABLE_FILTERING_DOWNLINK          0x00
#define SIDEWALK_DISABLE_FILTERING_DOWNLINK         0x01

#define SIDEWALK_STOP_DATA_PAYLOAD                  0x01


/**
 * @brief set the value 
 *  0 to disable the trace buffer and 1 to enable the trace buffer
 * 
 */
#define ENABLE_TRACE_BUFFER                         1

#define TRACE_INFO(...)                             do                              \
                                                    {                               \
                                                        if (ENABLE_TRACE_BUFFER)    \
                                                            printf(__VA_ARGS__);    \
                                                    } while (0);



/**********************************************************************************************************
 * TYPEDEFS
 **********************************************************************************************************/

/*
    OxTech MCM User Guide
    4.4 Message Format
*/
typedef enum {
    COMMAND_TYPE_GENERAL    = 0x01,
    COMMAND_TYPE_LORAWAN    = 0x02,
    COMMAND_TYPE_SIDEWALK   = 0x03
}command_types_t;


/**
 * @brief Oxtech mcm user guide 4.4.2.1
 * 
 */
typedef enum 
{
    MROVER_RC_OK                   = 0x00,  //!< command executed without errors
    MROVER_RC_UNKNOWN              = 0x01,  //!< command code unknown
    MROVER_RC_NOT_IMPLEMENTED      = 0x02,  //!< command not implemented
    MROVER_RC_FAIL                 = 0x06,  //!< command execution failed
    MROVER_RC_BAD_CRC              = 0x08,  //!< CRC check failed  
    MROVER_RC_BAD_SIZE             = 0x0A,  //!< size check failed
    MROVER_RC_NOTIFY_EVENTS        = 0x20   //!< command executed without errors
} mrover_return_code_t;

/**
 * @brief Oxtech mcm user guide 4.5.1
 * 
 */
typedef enum{

    MROVER_CC_GET_EVENT                            = 0x0000,   //Retrieve pending events
    MROVER_CC_GET_VERSION                          = 0x0001,   // get version of bootloader, application hardware,sidewalk and lorawan
    MROVER_CC_RESET                                = 0x0002,   // reset MCM module
    MROVER_CC_FACTORY_RESET                        = 0x0003,   // factory reset
    MROVER_CC_SWITCH_NETWORK                       = 0x0100,   // switch network between sidewalk and lorawan
    MROVER_CC_INIT_LORAWAN                         = 0x00FF,   // initialize lorawan
    MROVER_CC_SET_JOIN_EUI                         = 0x0011,   // set join eui for lorawan
    MROVER_CC_SET_DEV_EUI                          = 0x0013,   // set dev eui for lorawan
    MROVER_CC_SET_NW_KEY                           = 0x0014,   // set network key for lorawan
    MROVER_CC_GET_DEV_EUI                          = 0x0012,   // get dev eui for lorawan
    MROVER_CC_GET_JOIN_EUI                         = 0x0010,   // get join eui for lorawan
    MROVER_CC_JOIN_LORAWAN                         = 0x0025,   // join lorawan
    MROVER_CC_REQUEST_UPLINK                       = 0x0029,   // transmit command to send uplink
    MROVER_CC_LEAVE_LORAWAN_NETWORK                = 0x0026,   // leave lorawan network
    MROVER_CC_STOP_SID_LORAWAN_NETWORK             = 0x00FE,   // stop lorawan network
    MROVER_CC_BLE_LINK_REQUEST                     = 0x00FA,   // initialize ble link
    MROVER_CC_BLE_CONNECTION_REQUEST               = 0x00FB,   // ble request to transmit/receive data
    MROVER_CC_FSK_LINK_REQUEST                     = 0x00F8,   // initialize fsk link
    MROVER_CC_CSS_LINK_REQUEST                     = 0x00F9,   // request CSS link
    MROVER_CC_SET_CSS_PWR_PROFILE                  = 0x00FD,   // set power profile A or B for CSS
    MROVER_CC_SET_FILTERING_DOWNLINK_SIDEWALK      = 0x00FC,   // enable/disable duplicate downlink  
    MROVER_CC_GET_LORAWAN_CLASS                    = 0x0015,   // get lorawan class
    MROVER_CC_SET_LORAWAN_CLASS                    = 0x0016,   // set lorawan class
    MROVER_CC_START_FILE_TRANSFER                  = 0x00D3,   // start file transfer
    MROVER_CC_FILE_STATUS                          = 0x00D4,   // get file status
    MROVER_CC_TRIGGER_FW_UPDATE                    = 0x00D5,   // trigger firmware update
}mrover_cc_codes_t;

/**
 * @brief Type of the event returned by the mcm module 
 *         Oxtech guide 4.5.3.1 
 */
typedef enum{
    MODEM_EVENT_RESET                     = 0x00, //!< Modem has been reset
    MODEM_EVENT_ALARM                     = 0x01, //!< Alarm timer expired
    MODEM_EVENT_JOINED                    = 0x02, //!< Network successfully joined
    MODEM_EVENT_TXDONE                    = 0x03, //!< Frame transmitted
    MODEM_EVENT_DOWNDATA                  = 0x04, //!< Downlink data received
    MODEM_EVENT_UPLOADDONE                = 0x05, //!< File upload completed
    MODEM_EVENT_SETCONF                   = 0x06, //!< Configuration has been changed by the Device Management
    MODEM_EVENT_MUTE                      = 0x07, //!< Modem has been muted or un-muted by the Device Management
    MODEM_EVENT_STREAMDONE                = 0x08, //!< Stream upload completed (stream data buffer depleted)
    MODEM_EVENT_JOINFAIL                  = 0x0A, //!< Attempt to join network failed
    MODEM_EVENT_TIME                      = 0x0D, //!< Update on time happened (synced or invalid)
    MODEM_EVENT_TIMEOUT_ADR_CHANGED       = 0x0E, //!< ADR profile was switched to network controlled
    MODEM_EVENT_NEW_LINK_ADR              = 0x0F, //!< New link ADR requested by network
    MODEM_EVENT_LINK_CHECK                = 0x10, //!< Link Check answered by network
    MODEM_EVENT_ALMANAC_UPDATE            = 0x11, //!< An almanac update has been received
    MODEM_EVENT_USER_RADIO_ACCESS         = 0x12, //!< radio callback when user use the radio by itself
    MODEM_EVENT_CLASS_B_PING_SLOT_INFO    = 0x13, //!< Ping Slot Info answered by network
    MODEM_EVENT_CLASS_B_STATUS            = 0x14, //!< Downlink class B is ready or not
    MODEM_EVENT_LORAWAN_MAC_TIME          = 0x15, //!< Device Time is ready or not
    MODEM_EVENT_SEGMENTED_FILE_DOWNLOAD   = 0xD0, //!< Event to notify Host about successful file (full) and segments transfer
    MODEM_EVENT_CLASS_SWITCHED            = 0xF0, //!< LORAWAN Class has been switched
    MODEM_EVENT_NONE                      = 0xFF  //!< No event available
}get_event_code_t;

/**
 * @brief Set the uplink frame to be confirmed or unconfirmed
 * 
 */
 typedef enum {
    MROVER_UNCONFIRMED_UPLINK   = 0x00,
    MROVER_CONFIRMED_UPLINK     = 0x01
 }mrover_uplink_type_t;

 /**
  * @brief Event received from the module regarding the last uplink
  */
 typedef enum {
    MROVER_TX_NOT_SEND              = 0x00,
    MROVER_TX_DONE_WITHOUT_ACK      = 0x01,
    MROVER_TX_DONE_WITH_ACK         = 0x02
 }mrover_uplink_event_type_t;

/**
 * @brief Set the CSS power profile 
 * Oxit mcm guide 4.5.3.20
 * 
 */
 typedef enum {
    MROVER_CSS_PWR_PROFILE_A       = 0x00,
    MROVER_CSS_PWR_PROFILE_B       = 0x01
 }mrover_css_pwr_profile_t;


/**
 * @brief When enable, duplicates are filtered and are not propagated
 *        When disable duplicates are not detected
 */
 typedef enum {
    MROVER_SID_ENABLE_FILTERING    = 0x00,
    MROVER_SID_DISABLE_FILTERING   = 0x01
 }mrover_sid_downlink_filter_t;

/**
 * @brief LoRaWAN device class types
 */
typedef enum {
    MROVER_LORAWAN_CLASS_A       = 0x00,
    MROVER_LORAWAN_CLASS_B       = 0x01,
    MROVER_LORAWAN_CLASS_C       = 0x02
} mrover_lorawan_class_t;


/**********************************************************************************************************
 * ExPORTED VARIABLES
 **********************************************************************************************************/

/**********************************************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 **********************************************************************************************************/

#ifdef __cplusplus
}
#endif
#endif // __COMMANDS_DEFS_H__