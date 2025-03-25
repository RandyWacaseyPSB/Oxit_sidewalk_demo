/**
 * @file mcm_rover.h
 * @author Ankit Bansal (ankit.bansal@oxit.com)
 * @brief Header file for the mcm rover 
 * @version 0.1
 * @date 2024-05-20
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


#ifndef __MCM_ROVER_H__
#define __MCM_ROVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
 * INCLUDES
 **********************************************************************************************************/
#include <Arduino.h>
#include <stdint.h>
#include "api_processor.h" 
#include "ymodem.h"
#include "host_fuota.h"

/**********************************************************************************************************
 * MACROS AND DEFINES
 **********************************************************************************************************/

/**
 * @brief size for the buffer 
 * mcm can send upto 306 bytes as response
 * 6 bytes as notification (but adding up 15 bytes for safety)
 * This would needed for y moden protocol
 */
#define BUFFER_SIZE (1036)

#define MCM_ROVER_LIB_VER_MAJOR 0
#define MCM_ROVER_LIB_VER_MINOR 3
#define MCM_ROVER_LIB_VER_PATCH 0

/**********************************************************************************************************
 * TYPEDEFS AND CLASSES
 **********************************************************************************************************/

enum class MCM_STATUS {
    MCM_OK,
    MCM_ERROR,
    MCM_PARAM_ERROR,
    MCM_TIMEOUT
};

enum class ConnectionMode {
    CONNECTION_MODE_NC,
    CONNECTION_MODE_LORAWAN,
    CONNECTION_MODE_SIDEWALK_BLE,
    CONNECTION_MODE_SIDEWALK_FSK,
    CONNECTION_MODE_SIDEWALK_CSS,
};

enum class PayloadType {
    MCM_SW_VER,
    LORAWAN_DEV_EUI,
    LORAWAN_JOIN_EUI,
    LORAWAN_APP_KEY
};

enum class MCM_TX_STATUS {
    MCM_TX_NOT_SEND,
    MCM_TX_WO_ACK,
    MCM_TX_ACK
};

enum class MCM_UPLINK_TYPE{
    MCM_UPLINK_TYPE_NA,
    MCM_UPLINK_TYPE_CONF,
    MCM_UPLINK_TYPE_UNCONF
};

enum class MCM_LORAWAN_CLASS_TYPE
{
    MCM_LRWAN_CLASS_A = 0x00,
    MCM_LRWAN_CLASS_B = 0x01,
    MCM_LRWAN_CLASS_C = 0X02
};

typedef void(*on_rx_callback)(uint8_t *data, uint8_t len,int8_t rssi,uint8_t snr,uint16_t seq_port);


class MCM {

    private:
    
    ConnectionMode current_mode = ConnectionMode::CONNECTION_MODE_NC;
    uint8_t _reset_pin;
    uint32_t _baud_rate;
    uint8_t _rx_pin;
    uint8_t _tx_pin;
    HardwareSerial& __mcm_serial;
    mcm_module_hdl_t *module = NULL;
    uint8_t is_rx_received;
    uint16_t received_size;
    uint8_t sw_reset_evnet_count;
    bool is_joined_network;
    MCM_TX_STATUS last_tx_status;
    bool is_downlink_avail =  false;
    bool is_last_uplink_pend;
    uint8_t downlink_buffer[BUFFER_SIZE];
    uint16_t downlink_buffer_size;
    on_rx_callback on_rx_callback_func = nullptr;
    uint32_t serial_rx_timeout = 2000;
    int8_t rssi;
    uint8_t snr;
    uint16_t seq_port;
    bool is_debug_enabled = false;
    bool _context_mgr_is_joined_cmd_received = false;
    bool _context_mgr_is_mcm_reset;
    void process_received_data();
    
public:
    ver_type_1_t host_version;
    YModem ymodem;
    bool is_new_firmware_downloaded = false;
    get_seg_file_status_t seg_file_status;
    MCM_LORAWAN_CLASS_TYPE _dev_class = MCM_LORAWAN_CLASS_TYPE::MCM_LRWAN_CLASS_A;
    bool _change_class = false;
    MCM(HardwareSerial& serial,uint8_t tx_pin, uint8_t rx_pin, uint8_t reset_pin);
    MCM_STATUS begin();
    String print_version();
    void sw_reset();
    void hw_reset();
    void set_connect_mode(ConnectionMode mode);
    ConnectionMode get_connect_mode();
    MCM_STATUS set_lorawan_credentials(uint8_t *dev_eui, uint8_t *join_eui,uint8_t *app_key);
    MCM_STATUS connect_network();
    void send_uplink(uint8_t *data, uint16_t len,uint8_t port,MCM_UPLINK_TYPE send_uplink);
    void handle_rx_events();
    bool is_connected();
    MCM_TX_STATUS get_last_tx_status();
    bool is_last_uplink_pending();
    void set_on_rx_callback(on_rx_callback callback);
    void stop_network();
    void set_serial_rx_timeout(uint32_t timeout);
    bool is_downlink_available();
    void get_downlink_data(uint8_t *data, uint16_t* len,int8_t* rssi,int8_t* snr,uint16_t* seq_port);
    void set_debug_enabled(bool val);
    MCM_STATUS set_lorawan_class(MCM_LORAWAN_CLASS_TYPE dev_class);
    MCM_LORAWAN_CLASS_TYPE get_lorawan_class();
    MCM_STATUS factory_reset();
    mcm_module_hdl_t* get_module_handle();
    HardwareSerial& get_serial();
    void set_is_rx_received(uint8_t val);
    void set_received_size(uint16_t val);
    uint8_t get_sw_reset_event_count();
    void set_is_joined_network(bool val);
    void set_is_last_uplink_pending(bool val);
    uint8_t* get_downlink_buffer();
    on_rx_callback get_on_rx_callback_func();
    void set_last_tx_status(MCM_TX_STATUS status);
    void set_downlink_meta_data(uint16_t len,int8_t rssi,int8_t snr,uint16_t seq_port,bool val);
    bool get_is_debug_enabled();
    void set_context_mgr_is_joined_cmd_received(bool val);
    bool get_context_mgr_is_joined_cmd_received();
    void set_context_mgr_is_mcm_reset(bool val);
    bool get_context_mgr_is_mcm_reset();
    bool get_context_mgr_is_class_change();
    MCM_STATUS start_file_transfer(ver_type_1_t version);
    MCM_STATUS get_segmented_file_download_status(get_seg_file_status_t *status);
    MCM_STATUS trigger_firmware_update(ver_type_1_t version);
    bool is_new_firmware();
    void set_host_app_version(ver_type_1_t version);
    void retrieveLibraryVersions(ver_type_1_t *mcm_rover_lib_ver, ver_type_1_t *c_lib_ver);
    MCM_STATUS process_fw_update();

};

/**********************************************************************************************************
 * EXPORTED VARIABLES
 **********************************************************************************************************/

/**********************************************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 **********************************************************************************************************/

#ifdef __cplusplus
}
#endif
#endif // __MCM_ROVER_H__