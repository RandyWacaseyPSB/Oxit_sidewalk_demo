/**
 * @file mcm_rover.cpp
 * @author Ankit Bansal (ankit.bansal@oxit.com)
 * @brief Implementation for the MCM Rover class
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

/******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <Arduino.h>
#include <cstdio>
#include "mcm_rover.h"
#include "host_fuota.h"

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
uint8_t temp_buffer[BUFFER_SIZE];
String version;

// TODO: remove it after the mcm reset bug fixed
//  currently there is a bug,due to which
// mcm gives 2 events for software reset
//  this is a workaround
uint16_t reset_event_count = 0;
/******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

/******************************************************************************
 * STATIC FUNCTION PROTOTYPES
 ******************************************************************************/

/******************************************************************************
 * STATIC FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/

/******************************************************************************
 * Function Definitions
 *******************************************************************************/

static uint16_t on_send_function(uint8_t *data, uint16_t size, void *ctx)
{
    MCM *curr_instance = (MCM *)ctx;
    curr_instance->set_received_size(0);
    curr_instance->set_is_rx_received(0);
    Serial.printf("HMI TX: (%d Bytes)", size);
    for (int i = 0; i < size; i++)
    {
        Serial.printf(" %02x", data[i]);
    }
    Serial.println("");
    return (uint16_t)curr_instance->get_serial().write(data, size);
}

static void handle_notification(void *ctx)
{
    MCM *curr_instance = (MCM *)ctx;
    // Host application could do any relevant action based on the notification
    if (curr_instance->get_is_debug_enabled())
    {
        Serial.printf("Notification received\n");
        Serial.printf("Number of pending events are %d\n", api_processor_get_pending_events(curr_instance->get_module_handle()));
    }
} 

static void handle_mcm_response(const api_processor_response_t *mcm_response, void *ctx)
{
    MCM *curr_instance = (MCM *)ctx;

    if (curr_instance->get_is_debug_enabled())
        Serial.printf("Response received\n");

    /**
     * @brief Using the return code we can analyze that the last command was successful or not
     * we can also add the error handling in case we need to do anything for the failed command
     *
     */
    if (MROVER_RC_OK != mcm_helper_get_response_code(mcm_response))
    {
        if (curr_instance->get_is_debug_enabled())
        {
            Serial.printf("Last command failed with error code %d\n", mcm_response->return_code);
            Serial.printf("Last command is 0x%04x\n", mcm_response->cmd_code);
        }
        mrover_cc_codes_t cmd_code = mcm_helper_get_command_code(mcm_response);
        if(MROVER_CC_FILE_STATUS == cmd_code)
        {
          Serial.printf("No file found.\n");
        }
        return;
    }

    /**
     * @brief Command code describe what is the last command we send
     * for which we are looking the response which is successful, for other response
     * user above function
     * cmd_response_data contains all the data related to the command itself.
     * it also contain the sub events if there are any and the data associates with them
     */
    switch (mcm_helper_get_command_code(mcm_response))
    {
    case MROVER_CC_GET_EVENT:
    {
        Serial.printf("MROVER_CC_GET_EVENT\n");
        // get event command always carries the number of pending events to be read
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Number of pending events are %d\n", api_processor_get_pending_events(curr_instance->get_module_handle()));

        /**
         * @brief Now the command response data contains the data related to the command.
         * each command code has its own data structure, if any
         * In the case of get event command, the data is in the get_event_data structure
         * get_event_data contain the 2 information, the get event code and the data associate with the event code, in get_event_data_value
         *
         */
        switch (mcm_helper_get_event_code(mcm_response))
        {
        case MODEM_EVENT_RESET:
        {
            Serial.printf("MODEM_EVENT_RESET\n");
            if (curr_instance->get_is_debug_enabled())
            {
                Serial.printf("Response of the reset command has been received\n");
                /**
                 * @brief data related to the get event has been in the get_event_data_value,
                 * The reset event contain its own data set in the reset data structure, which has single value of reset count
                 *
                 */
                Serial.printf("Reset count %d\n", mcm_helper_get_event_reset_count(mcm_response));
            }
            reset_event_count++;
            // Serial.printf("Reset count %d\n", reset_event_count);
            // if(reset_event_count <= 2)
            //     break;

            // reset the module
            // set the joined status to false in case if device is resetted
            curr_instance->set_is_joined_network(false);

            if (curr_instance->get_context_mgr_is_joined_cmd_received())
            {
                // set the flag that device rebooted and let the application layer know
                curr_instance->set_context_mgr_is_mcm_reset(true);
            }
        }
        break;

        /**
         * @brief Time sync happens when either device joins to the lorawan or
         *         device is time synced with the sidewalk network
         *        This symbolize the uplink is ready to send
         */
        case MODEM_EVENT_JOINED:
            /**
             * @brief if the command type is the lorawan, then the
             * device is joined to the lorawan network
             * otherwise sidewalk has been timed synced
             * No data is associated with this event type
             */
            {
                Serial.printf("MODEM_EVENT_JOINED\n");
                if (COMMAND_TYPE_LORAWAN == mcm_helper_get_command_type(mcm_response))
                {
                    if (curr_instance->get_is_debug_enabled())
                        Serial.printf("Device has been successfully joined to lorawan network\n");
                }
                else if (COMMAND_TYPE_SIDEWALK == mcm_response->cmd_type)
                {
                    if (curr_instance->get_is_debug_enabled())
                        Serial.printf("Device has been successfully time synced to sidewalk network\n");
                }

                curr_instance->set_is_joined_network(true);
            }
            break;

        /**
         * @brief if the command type is the lorawan, then the
         * device is failed joined to the lorawan network
         * otherwise sidewalk  timed synced failed.
         *
         */
        case MODEM_EVENT_JOINFAIL:
        {
            Serial.printf("MODEM_EVENT_JOINFAIL\n");
            if (COMMAND_TYPE_LORAWAN == mcm_helper_get_command_type(mcm_response))
            {
                if (curr_instance->get_is_debug_enabled())
                    Serial.printf("Join fail event occurred\n");
            }
            else if (COMMAND_TYPE_SIDEWALK == mcm_helper_get_command_type(mcm_response))
            {
                if (curr_instance->get_is_debug_enabled())
                    Serial.printf("Sidewalk time sync fail event\n");
            }
            curr_instance->set_is_joined_network(false);
        }
        break;
        /**
         * @brief When ever is the status of the tx link has been updated
         *          It has been reported here.
         *          Device sends the notification about the tx link(uplink) status
         *          We send the get event to read what type of event occurred
         *
         */
        case MODEM_EVENT_TXDONE:
            /**
             * @brief Now get the tx status in the event data value
             *
             */
            {
                Serial.printf("MODEM_EVENT_TXDONE\n");
                curr_instance->set_is_last_uplink_pending(false);
                switch (mcm_helper_get_event_tx_status(mcm_response))
                {
                case MROVER_TX_NOT_SEND:
                    Serial.printf("MROVER_TX_NOT_SEND\n");
                    if (curr_instance->get_is_debug_enabled())
                        Serial.printf("Unable to send the last packet\n");
                    curr_instance->set_last_tx_status(MCM_TX_STATUS ::MCM_TX_NOT_SEND);
                    break;

                case MROVER_TX_DONE_WITHOUT_ACK:
                    Serial.printf("MROVER_TX_DONE_WITHOUT_ACK\n");
                    if (curr_instance->get_is_debug_enabled())
                        Serial.printf("Last Tx done with no ack\n");
                    curr_instance->set_last_tx_status(MCM_TX_STATUS ::MCM_TX_WO_ACK);
                    break;

                case MROVER_TX_DONE_WITH_ACK:
                    Serial.printf("MROVER_TX_DONE_WITH_ACK\n");
                    if (curr_instance->get_is_debug_enabled())
                        Serial.printf("Last Tx done with ack\n");
                    curr_instance->set_last_tx_status(MCM_TX_STATUS ::MCM_TX_ACK);
                    break;

                default:
                    break;
                }
            }

            break;

        case MODEM_EVENT_DOWNDATA:
        {
            Serial.printf("MODEM_EVENT_DOWNDATA\n");
            if (curr_instance->get_is_debug_enabled())
                Serial.printf("downlink has been received\n");
            int8_t rssi;
            int8_t snr;
            uint16_t payload_len = mcm_helper_get_downlink_len(mcm_response); // first determine the downlink length of the received data
            uint16_t seq_port;
            uint8_t *payload = curr_instance->get_downlink_buffer();

            mcm_helper_get_downlink_data(mcm_response, &rssi, &snr, payload, &seq_port);

            curr_instance->set_downlink_meta_data(payload_len, rssi, snr, seq_port, true);
            if (curr_instance->get_is_debug_enabled())
            {
                Serial.printf("Rssi: %d\n", rssi);
                Serial.printf("Snr: %d\n", snr);
                if (COMMAND_TYPE_LORAWAN == mcm_helper_get_command_type(mcm_response)) // helper function to get the command type
                {
                    Serial.printf("Port %d\n", seq_port);
                }

                else
                {
                    Serial.printf("Sequence %d\n", seq_port);
                    // since we received the sidewalk downlink, we can stop the sidewalk uplink
                }

                Serial.printf("Payload: ");
                for (int i = 0; i < payload_len; i++)
                {
                    Serial.printf("0x%02x,", payload[i]);
                }
                Serial.printf("\n");
            }

            if (nullptr != curr_instance->get_on_rx_callback_func())
            {
                curr_instance->get_on_rx_callback_func()(payload, payload_len, rssi, snr, seq_port);
            }
        }
        break;
        case MODEM_EVENT_CLASS_SWITCHED:
        {
            Serial.printf("MODEM_EVENT_CLASS_SWITCHED\n");
            curr_instance->_change_class = true;
            switch (mcm_helper_get_event_new_class(mcm_response))
            {
            case MROVER_LORAWAN_CLASS_A:
                Serial.printf("MROVER_LORAWAN_CLASS_A\n");
                curr_instance->_dev_class = MCM_LORAWAN_CLASS_TYPE::MCM_LRWAN_CLASS_A;
                break;

            case MROVER_LORAWAN_CLASS_B:
                Serial.printf("MROVER_LORAWAN_CLASS_B\n");
                curr_instance->_dev_class = MCM_LORAWAN_CLASS_TYPE::MCM_LRWAN_CLASS_B;
                break;

            case MROVER_LORAWAN_CLASS_C:
                Serial.printf("MROVER_LORAWAN_CLASS_C\n");
                curr_instance->_dev_class = MCM_LORAWAN_CLASS_TYPE::MCM_LRWAN_CLASS_C;
                break;
            }
            if (curr_instance->get_is_debug_enabled())
            {
                Serial.printf("Class switch event received\n");
                Serial.printf("New class is %c\r\n", "ABC"[mcm_helper_get_event_new_class(mcm_response)]);
            }
        }
        break;
        case MODEM_EVENT_SEGMENTED_FILE_DOWNLOAD:
        {
            Serial.printf("MODEM_EVENT_SEGMENTED_FILE_DOWNLOAD\n");
            if (curr_instance->get_is_debug_enabled())
                Serial.printf("Segmented downlink event received\n");

            curr_instance->seg_file_status = mcm_helper_get_event_seg_down(mcm_response);
            Serial.printf("binary file type: %d\n", curr_instance->seg_file_status.cmd_type.bin_type);
            Serial.printf("Firmware Version: %d.%d.%d\n",
                          curr_instance->seg_file_status.fw_ver.major,
                          curr_instance->seg_file_status.fw_ver.minor,
                          curr_instance->seg_file_status.fw_ver.patch);
            Serial.printf("Package Size: %d %d %d\n",
                          curr_instance->seg_file_status.pkg_size[0],
                          curr_instance->seg_file_status.pkg_size[1],
                          curr_instance->seg_file_status.pkg_size[2]);
            Serial.printf("Segment Size: %d\n", curr_instance->seg_file_status.seg_size);
            Serial.printf("Segment ID: %d\n", curr_instance->seg_file_status.nxt_seg_id);
            Serial.printf("Segment Status: 0x%04x\n", curr_instance->seg_file_status.seg_status);
            Serial.printf("Checking if all segments are downloaded\n");

            // Check if all segments are downloaded
            if (is_all_segments_downloaded(curr_instance->seg_file_status))
            {
                Serial.printf("All segments downloaded\n");

                // Debug prints to track binary type and version comparison
                if (curr_instance->seg_file_status.cmd_type.bin_type == FUOTA_BINARY_TYPE_HOST)
                {
                    Serial.printf("Binary is for host, comparing versions\n");
                    Serial.printf("Current Host Version: %d.%d.%d\n",
                                  curr_instance->host_version.major,
                                  curr_instance->host_version.minor,
                                  curr_instance->host_version.patch);
                    Serial.printf("Downloaded Firmware Version: %d.%d.%d\n",
                                  curr_instance->seg_file_status.fw_ver.major,
                                  curr_instance->seg_file_status.fw_ver.minor,
                                  curr_instance->seg_file_status.fw_ver.patch);

                    if (curr_instance->seg_file_status.fw_ver.major != curr_instance->host_version.major ||
                        curr_instance->seg_file_status.fw_ver.minor != curr_instance->host_version.minor ||
                        curr_instance->seg_file_status.fw_ver.patch != curr_instance->host_version.patch)
                    {
                        Serial.printf("New firmware version detected\n");
                        curr_instance->is_new_firmware_downloaded = true;
                    }
                    else
                    {
                        Serial.printf("No new firmware version detected\n");
                    }
                }
                else
                {
                    Serial.printf("Binary is not for host, new firmware downloaded\n");
                    curr_instance->is_new_firmware_downloaded = true;
                }
            }
            else
            {
                Serial.printf("Not all segments are downloaded yet\n");
            }
        }
        break;

        case MODEM_EVENT_NONE:
            Serial.printf("MODEM_EVENT_NONE\n");
            // this will indicate that no pending events
            if (curr_instance->get_is_debug_enabled())
                Serial.printf("No event\n");
            break;
        default:
            break;
        }
    }
    break;

    /**
     * @brief Response of the get version command
     *
     */
    case MROVER_CC_GET_VERSION:
    {
        Serial.printf("MROVER_CC_GET_VERSION\n");
        ver_type_2_t bootloader;
        ver_type_2_t modem_fw;
        ver_type_1_t modem_hw;
        ver_type_1_t sidewalk;
        ver_type_1_t lorawan;

        mcm_helper_get_version(mcm_response, &bootloader, &modem_fw, &modem_hw, &sidewalk, &lorawan);
        version = "";
        char data[100];
        sprintf(data, "Bootloader: %d.%d.%d\n", bootloader.major, bootloader.minor, bootloader.patch);
        version += data;
        sprintf(data, "Modem firmware: %d.%d.%d\n", modem_fw.major, modem_fw.minor, modem_fw.patch);
        version += data;
        sprintf(data, "Modem hardware: %d.%d.%d\n", modem_hw.major, modem_hw.minor, modem_hw.patch);
        version += data;
        sprintf(data, "Sidewalk: %d.%d.%d\n", sidewalk.major, sidewalk.minor, sidewalk.patch);
        version += data;
        sprintf(data, "Lorawan: %d.%d.%d\n", lorawan.major, lorawan.minor, lorawan.patch);
        version += data;
        if (curr_instance->get_is_debug_enabled())
        {
            Serial.printf("Bootloader: %d.%d.%d\n", bootloader.major, bootloader.minor, bootloader.patch);
            Serial.printf("Modem firmware: %d.%d.%d\n", modem_fw.major, modem_fw.minor, modem_fw.patch);
            Serial.printf("Modem hardware: %d.%d.%d\n", modem_hw.major, modem_hw.minor, modem_hw.patch);
            Serial.printf("Sidewalk: %d.%d.%d\n", sidewalk.major, sidewalk.minor, sidewalk.patch);
            Serial.printf("Lorawan: %d.%d.%d\n", lorawan.major, lorawan.minor, lorawan.patch);
        }
    }
    break;

    /**
     * @brief Response of the reset command.
     * After sending the reset command, device would send the notification
     * regarding the reset event.
     *
     */
    case MROVER_CC_RESET:
        Serial.printf("MROVER_CC_RESET\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Command successfully received. Device would be reset only when there is no pending event\n");
        break;

    /**
     * @brief Response for the factory reset command
     *
     */
    case MROVER_CC_FACTORY_RESET:
        Serial.printf("MROVER_CC_FACTORY_RESET\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Device has been factory reset\n");
        break;

    /**
     * @brief Response of the completion of switch command
     *
     */
    case MROVER_CC_SWITCH_NETWORK:
        Serial.printf("MROVER_CC_SWITCH_NETWORK\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Device has been switched successfully\n");
        break;

    /**
     * @brief Response of the initialization
     * of lorawan command
     *
     */
    case MROVER_CC_INIT_LORAWAN:
        Serial.printf("MROVER_CC_INIT_LORAWAN\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Lorawan has been initiated successfully\n");
        break;

    /**
     * @brief Response for the successful setting of
     * join eui command.
     *
     */
    case MROVER_CC_SET_JOIN_EUI:
        Serial.printf("MROVER_CC_SET_JOIN_EUI\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Join eui has been set successfully\n");
        break;

    /**
     * @brief Response for the successful setting of
     * dev eui command
     *
     */
    case MROVER_CC_SET_DEV_EUI:
        Serial.printf("MROVER_CC_SET_DEV_EUI\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Dev eui has been set successfully\n");
        break;
    /**
     * @brief Response for the successful setting of
     * network key
     *
     */
    case MROVER_CC_SET_NW_KEY:
        Serial.printf("MROVER_CC_SET_NW_KEY\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Network key has been set successfully\n");
        break;

    /**
     * @brief Response for the successful get dev eui
     *
     */
    case MROVER_CC_GET_DEV_EUI:
    {
        Serial.printf("MROVER_CC_GET_DEV_EUI\n");
        // read the dev eui here
        const uint8_t *dev_eui = mcm_helper_get_dev_eui(mcm_response);
        if (curr_instance->get_is_debug_enabled())
        {
            Serial.printf("Dev eui: ");
            for (int i = 0; i < LORAWAN_DEV_EUI_JOIN_EUI_LEN; i++)
            {
                Serial.printf("0x%02x,", dev_eui[i]);
            }
            Serial.printf("\n");
        }
    }

    break;

    /**
     * @brief Response for the successful get join eui
     *
     */
    case MROVER_CC_GET_JOIN_EUI:
    {
        Serial.printf("MROVER_CC_GET_JOIN_EUI\n");
        // read the join eui here
        const uint8_t *join_eui = mcm_helper_get_join_eui(mcm_response);
        if (curr_instance->get_is_debug_enabled())
        {
            Serial.printf("Join eui: ");
            for (int i = 0; i < LORAWAN_DEV_EUI_JOIN_EUI_LEN; i++)
            {
                Serial.printf("0x%02x,", join_eui[i]);
            }
            Serial.printf("\n");
        }
    }
    break;

    /**
     * @brief Response of the join lorawan command
     *        This just indicates that the device has successfully received the command
     *          Result of this command would be responded by, MODEM_EVENT_JOINED, sub event of
     *          MROVER_CC_GET_EVENT
     */
    case MROVER_CC_JOIN_LORAWAN:
        Serial.printf("MROVER_CC_JOIN_LORAWAN\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Command to join lorawan executed successfully, Module will send us  event as soon we joined\n");
        break;

    /**
     * @brief Response of the uplink command
     *        This just indicates that the device has successfully received the command
     *        Result of this command would be responded by, MODEM_EVENT_TXDONE, sub event of  MROVER_CC_GET_EVENT
     */
    case MROVER_CC_REQUEST_UPLINK:
        Serial.printf("MROVER_CC_REQUEST_UPLINK\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Uplink has been requested successfully\n");
        break;

    /**
     * @brief Response command that device has successful leave the network
     *
     */
    case MROVER_CC_LEAVE_LORAWAN_NETWORK:
        Serial.printf("MROVER_CC_LEAVE_LORAWAN_NETWORK\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Device has been left the lorawan network\n");
        break;

    /**
     * @brief Response of the command when device comes out of the
     *        Lorawan or sidewalk network
     *
     */
    case MROVER_CC_STOP_SID_LORAWAN_NETWORK:
    {
        Serial.printf("MROVER_CC_STOP_SID_LORAWAN_NETWORK\n");
        if (curr_instance->get_is_debug_enabled())
        {
            if (COMMAND_TYPE_SIDEWALK == mcm_helper_get_command_type(mcm_response))
            {
                Serial.printf("Device has stoped the sidewalk network\n");
            }
            else
            {
                Serial.printf("Device has stoped the lorawan network\n");
            }
        }
    }
    break;

    /**
     * @brief Response of the command when the device has requested the
     *        Ble link request
     *
     */
    case MROVER_CC_BLE_LINK_REQUEST:
    {
        Serial.printf("MROVER_CC_BLE_LINK_REQUEST\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Ble link has been requested successfully\n");
    }

    break;

    /**
     * @brief Response of the command when the device has requested the ble connection request
     *
     */
    case MROVER_CC_BLE_CONNECTION_REQUEST:
    {
        Serial.printf("MROVER_CC_BLE_CONNECTION_REQUEST\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Ble connection has been requested successfully\n");
    }
    break;

    /**
     * @brief Response of the command when device requested the ble fsk link
     *
     */
    case MROVER_CC_FSK_LINK_REQUEST:
        Serial.printf("MROVER_CC_FSK_LINK_REQUEST\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Ble FSK link has been requested successfully\n");
        break;

    /**
     * @brief Response of the command when device requested the css link
     *
     */
    case MROVER_CC_CSS_LINK_REQUEST:
        Serial.printf("MROVER_CC_CSS_LINK_REQUEST\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Css link has been requested successfully\n");
        break;

    /**
     * @brief Response for the set css power profile command
     *
     */
    case MROVER_CC_SET_CSS_PWR_PROFILE:
        Serial.printf("MROVER_CC_SET_CSS_PWR_PROFILE\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Css power profile has been set successfully\n");
        break;

    /**
     * @brief Response for the set filtering downlink command
     *
     */
    case MROVER_CC_SET_FILTERING_DOWNLINK_SIDEWALK:
        Serial.printf("MROVER_CC_SET_FILTERING_DOWNLINK_SIDEWALK\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Downlink filtering has been set successfully\n");
        break;

    case MROVER_CC_SET_LORAWAN_CLASS:
        Serial.printf("MROVER_CC_SET_LORAWAN_CLASS\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Command to change class has been successfully received by the mcm\n");
        break;
    case MROVER_CC_GET_LORAWAN_CLASS:
    {
        Serial.printf("MROVER_CC_GET_LORAWAN_CLASS\n");
        switch (mcm_helper_get_device_class(mcm_response))
        {
        case MROVER_LORAWAN_CLASS_A:
            Serial.printf("MROVER_LORAWAN_CLASS_A\n");
            curr_instance->_dev_class = MCM_LORAWAN_CLASS_TYPE::MCM_LRWAN_CLASS_A;
            break;

        case MROVER_LORAWAN_CLASS_B:
            Serial.printf("MROVER_LORAWAN_CLASS_B\n");
            curr_instance->_dev_class = MCM_LORAWAN_CLASS_TYPE::MCM_LRWAN_CLASS_B;
            break;

        case MROVER_LORAWAN_CLASS_C:
            Serial.printf("MROVER_LORAWAN_CLASS_C\n");
            curr_instance->_dev_class = MCM_LORAWAN_CLASS_TYPE::MCM_LRWAN_CLASS_C;
            break;
        }
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Command to Get class has been successfully received by the mcm\n");
        break;
    }
    case MROVER_CC_START_FILE_TRANSFER:
    {
        Serial.printf("MROVER_CC_START_FILE_TRANSFER\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Command to start file transfer has been successfully received by the mcm\n");
        // using the ymodem protocol
        curr_instance->ymodem.sendCRCRequest();
        curr_instance->ymodem.setState(WAIT_FOR_HEADER);
        break;
    }
    case MROVER_CC_FILE_STATUS:
    {
        Serial.printf("MROVER_CC_FILE_STATUS\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("File status event received\n");

        mcm_helper_get_seg_file_status(mcm_response, &curr_instance->seg_file_status);
        Serial.printf("binary file type: %d\n", curr_instance->seg_file_status.cmd_type.bin_type);
        Serial.printf("Firmware Version: %d.%d.%d\n",
                      curr_instance->seg_file_status.fw_ver.major,
                      curr_instance->seg_file_status.fw_ver.minor,
                      curr_instance->seg_file_status.fw_ver.patch);
        Serial.printf("Package Size: %d %d %d\n",
                      curr_instance->seg_file_status.pkg_size[0],
                      curr_instance->seg_file_status.pkg_size[1],
                      curr_instance->seg_file_status.pkg_size[2]);
        Serial.printf("Segment Size: %d\n", curr_instance->seg_file_status.seg_size);
        Serial.printf("Segment ID: %d\n", curr_instance->seg_file_status.nxt_seg_id);
        Serial.printf("Segment Status: 0x%04x\n", curr_instance->seg_file_status.seg_status);
        Serial.printf("Checking if all segments are downloaded\n");

        // Check if all segments are downloaded
        if (is_all_segments_downloaded(curr_instance->seg_file_status))
        {
            Serial.printf("All segments downloaded\n");

            // Debug prints to track binary type and version comparison
            if (curr_instance->seg_file_status.cmd_type.bin_type == FUOTA_BINARY_TYPE_HOST)
            {
                Serial.printf("Binary is for host, comparing versions\n");
                Serial.printf("Current Host Version: %d.%d.%d\n",
                              curr_instance->host_version.major,
                              curr_instance->host_version.minor,
                              curr_instance->host_version.patch);
                Serial.printf("Downloaded Firmware Version: %d.%d.%d\n",
                              curr_instance->seg_file_status.fw_ver.major,
                              curr_instance->seg_file_status.fw_ver.minor,
                              curr_instance->seg_file_status.fw_ver.patch);

                if (curr_instance->seg_file_status.fw_ver.major != curr_instance->host_version.major ||
                    curr_instance->seg_file_status.fw_ver.minor != curr_instance->host_version.minor ||
                    curr_instance->seg_file_status.fw_ver.patch != curr_instance->host_version.patch)
                {
                    Serial.printf("New firmware version detected\n");
                    curr_instance->is_new_firmware_downloaded = true;
                }
                else
                {
                    Serial.printf("No new firmware version detected\n");
                }
            }
            else
            {
                Serial.printf("Binary is not for host, new firmware downloaded\n");
                curr_instance->is_new_firmware_downloaded = true;
            }
        }
        else
        {
            Serial.printf("Not all segments are downloaded yet\n");
        }
    }
    break;

    case MROVER_CC_TRIGGER_FW_UPDATE:
    {
        Serial.printf("MROVER_CC_TRIGGER_FW_UPDATE\n");
        if (curr_instance->get_is_debug_enabled())
            Serial.printf("Command to trigger firmware update has been successfully received by the mcm\n");
    }
    break;
    default:
        break;
    }
}

MCM::MCM(HardwareSerial &serial, uint8_t tx_pin, uint8_t rx_pin, uint8_t reset_pin) : __mcm_serial(serial),
                                                                                      _tx_pin(tx_pin),
                                                                                      _rx_pin(rx_pin),
                                                                                      _reset_pin(reset_pin),
                                                                                      ymodem(serial)
{
}

MCM_STATUS MCM::begin()
{
    _baud_rate = 9600;
    pinMode(_reset_pin, OUTPUT);
    digitalWrite(_reset_pin, HIGH);
    __mcm_serial.begin(_baud_rate, SERIAL_8N1, _rx_pin, _tx_pin);
    __mcm_serial.setRxBufferSize(BUFFER_SIZE);
    __mcm_serial.setTxBufferSize(BUFFER_SIZE);
    // __mcm_serial.setRxTimeout(2);
    // keep in mind below function is lambda function
    __mcm_serial.onReceive([this]()
                           {
        //  size_t available = this->__mcm_serial.available();
        this->received_size = this->__mcm_serial.readBytes(temp_buffer, BUFFER_SIZE);
        if (this->get_is_debug_enabled())
        {
            Serial.println("on_receive_callback");
            Serial.printf("Received %d bytes\n", this->received_size);
        }
        this->is_rx_received = 1; }, true);
    if (this->get_is_debug_enabled())
        Serial.printf("mcm begin\n");

    // Initialize the module
    module = new mcm_module_hdl_t;
    module->user_context = this;
    // initialize the api processor
    api_processor_status_t status = api_processor_init(module,               // object for the mcm module
                                                       on_send_function,     // callback for the serial port send data callback
                                                       handle_notification,  // callback for the notification
                                                       handle_mcm_response); // callback for the mcm response

    if (API_PROCESSOR_SUCCESS == status)
    {
        return MCM_STATUS::MCM_OK;
    }

    return MCM_STATUS::MCM_ERROR;
}

String MCM::print_version()
{
    // send the request to get the version
    api_processor_cmd_get_version(this->module);

    this->process_received_data();
    return version;
}

void MCM::process_received_data()
{

    // wait until the response is received
    uint32_t timeout = millis();
    while ((this->is_rx_received == 0) &&
           ((millis() - timeout) < this->serial_rx_timeout))
    {
        delay(100);
    }
    if (this->is_rx_received == 0)
    {
        Serial.println("MCM: Response not received, Please check the connection");
    }
    this->is_rx_received = 0;
    if (this->get_is_debug_enabled())
    {
        Serial.println("---------------------------------Received debug info-------------------------------------");
    }
    
    //  if y-modem is enabled then send the data to the ymodem protocol only
    if (this->ymodem.getState() != YMODEM_IDLE) 
    {
      Serial.printf("YMODEM RX :(%d bytes) ", this->received_size);
      // for (int i = 0; i < this->received_size; i++) 
      // {
      //   Serial.printf("%02x ", temp_buffer[i]);
      // }
      Serial.println("");
      this->ymodem.receivePacket(temp_buffer, this->received_size);
      this->set_received_size(0);
      this->set_is_rx_received(0);
    } 
    else 
    {
      Serial.printf("HMI RX :(%d bytes) ", this->received_size);
      for (int i = 0; i < this->received_size; i++) 
      {
        Serial.printf("%02x ", temp_buffer[i]);
      }
      Serial.println("");
      api_processor_parse_rx_data(this->module, temp_buffer, this->received_size);  // TODO Oxit: Check if this can print just nothing
    }

    if (this->get_is_debug_enabled())
        Serial.println("-----------------------------------------------------------------------------------------");
}

void MCM::sw_reset()
{
    // send the request to reset the module
    api_processor_cmd_reset(this->module);

    this->process_received_data();

    // device would not be reset until we get all the event for the device
    while (api_processor_get_pending_events(this->module) > 0)
    {
        api_processor_cmd_get_event(this->module); // send the get event command

        this->process_received_data();
    }

    delay(1000);
}

void MCM::set_connect_mode(ConnectionMode mode)
{
    this->current_mode = mode;
}

ConnectionMode MCM::get_connect_mode()
{
    return this->current_mode;
}

MCM_STATUS MCM::set_lorawan_credentials(uint8_t *dev_eui, uint8_t *join_eui, uint8_t *app_key)
{
    MCM_STATUS status = MCM_STATUS::MCM_ERROR;
    api_processor_status_t api_status = API_PROCESSOR_ERROR;
    do
    {
        if (ConnectionMode::CONNECTION_MODE_LORAWAN != this->current_mode)
        {
            break;
        }

        // initiating the lorawan connection
        api_status = api_processor_cmd_init_lorawan(this->module);
        if (API_PROCESSOR_SUCCESS != api_status)
        {
            break;
        }
        // wait for the response
        this->process_received_data();

        // set dev eui
        api_status = api_processor_cmd_set_dev_eui(this->module, dev_eui, LORAWAN_DEV_EUI_JOIN_EUI_LEN);

        if (API_PROCESSOR_SUCCESS != api_status)
        {
            break;
        }
        // wait for the response
        this->process_received_data();
        // set join eui
        api_status = api_processor_cmd_set_join_eui(this->module, join_eui, LORAWAN_DEV_EUI_JOIN_EUI_LEN);
        if (API_PROCESSOR_SUCCESS != api_status)
        {
            break;
        }
        // wait for the response
        this->process_received_data();
        // set app key
        api_status = api_processor_cmd_set_nwk_key(this->module, app_key, LORAWAN_NETWORK_KEY_LEN);
        if (API_PROCESSOR_SUCCESS != api_status)
        {
            break;
        }
        // wait for the response
        this->process_received_data();
        status = MCM_STATUS::MCM_OK;

    } while (0);
    return status;
}

MCM_STATUS MCM::connect_network()
{
    MCM_STATUS status = MCM_STATUS::MCM_ERROR;
    api_processor_status_t api_status = API_PROCESSOR_ERROR;
    do
    {
        if (ConnectionMode::CONNECTION_MODE_NC == this->current_mode)
        {
            break;
        }
        else if (ConnectionMode::CONNECTION_MODE_LORAWAN == this->current_mode)
        {
            Serial.println("Initiating the lorawan connection");
            // initiating the lorawan connection
            api_status = api_processor_cmd_join_lorawan(this->module);
            if (API_PROCESSOR_SUCCESS != api_status)
            {
                break;
            }
            // wait for the response
            this->process_received_data();
            status = MCM_STATUS::MCM_OK;
        }

        else if (ConnectionMode::CONNECTION_MODE_SIDEWALK_BLE == this->current_mode)
        {
            // TODO: connect with sidwalk ble
            // connect with css
            api_status = api_processor_cmd_sid_ble_link_request(this->module);
            if (API_PROCESSOR_SUCCESS != api_status)
            {
                break;
            }
            // wait for the response
            this->process_received_data();
            status = MCM_STATUS::MCM_OK;
        }

        else if (ConnectionMode::CONNECTION_MODE_SIDEWALK_FSK == this->current_mode)
        {
            // connect with fsk
            api_status = api_processor_cmd_sid_fsk_link_request(this->module);
            if (API_PROCESSOR_SUCCESS != api_status)
            {
                break;
            }
            // wait for the response
            this->process_received_data();
            status = MCM_STATUS::MCM_OK;
        }

        else if (ConnectionMode::CONNECTION_MODE_SIDEWALK_CSS == this->current_mode)
        {
            // connect with css
            api_status = api_processor_cmd_sid_css_link_request(this->module);
            if (API_PROCESSOR_SUCCESS != api_status)
            {
                break;
            }
            // wait for the response
            this->process_received_data();
            status = MCM_STATUS::MCM_OK;
        }

        // set context manager that joined received
        this->set_context_mgr_is_joined_cmd_received(true);
    } while (0);
    return status;
}

void MCM::handle_rx_events()
{
    // check if any data to process
    // TODO: add the timeout here
    if (this->is_rx_received)
    {
        //Serial.println("handle_rx_events: RX data received");
        this->process_received_data();
        //Serial.println("handle_rx_events: Data processed");
        this->is_rx_received = 0;
    }
    // TODO: Oxit: process ymodem loop
    //  device would not be reset until we get all the event for the device
    if (this->ymodem.getState() != YMODEM_IDLE)
    {
        this->ymodem.process_timeout();
        while (this->__mcm_serial.available())
        {
            this->received_size = this->__mcm_serial.readBytes(temp_buffer, BUFFER_SIZE);
            Serial.printf("handle_rx_events: Read %d bytes from serial\n", this->received_size);
            this->is_rx_received = 1;
        }

        return;
    }
    
    while (api_processor_get_pending_events(this->module) > 0)
    {
        Serial.println("handle_rx_events: Pending events detected");
        api_processor_cmd_get_event(this->module); // send the get event command
        Serial.println("handle_rx_events: Get event command sent");

        this->process_received_data();
        Serial.println("handle_rx_events: Event data processed");
    }
}

bool MCM::is_connected()
{
    return this->is_joined_network;
}

bool MCM::is_last_uplink_pending()
{
    return this->is_last_uplink_pend;
}

MCM_TX_STATUS MCM::get_last_tx_status()
{
    return this->last_tx_status;
}

void MCM::send_uplink(uint8_t *data, uint16_t len, uint8_t port, MCM_UPLINK_TYPE send_uplink)
{
    this->is_last_uplink_pend = true;
    api_processor_status_t api_status = API_PROCESSOR_ERROR;

    /// Uplink Type conversion
    mrover_uplink_type_t uplink_type = MROVER_UNCONFIRMED_UPLINK;
    if (MCM_UPLINK_TYPE::MCM_UPLINK_TYPE_UNCONF == send_uplink)
    {
        uplink_type = MROVER_UNCONFIRMED_UPLINK;
    }
    else
    {
        uplink_type = MROVER_CONFIRMED_UPLINK;
    }

    /// Uplink will be done on the currently active network
    if (ConnectionMode::CONNECTION_MODE_LORAWAN == this->current_mode)
    {
        api_status = api_processor_cmd_request_lorawan_uplink(this->module, port, data, len, uplink_type);
    }
    else
    {
        /// BLE connection will be established before attempting an uplink for sidewalk BLE mode
        if (ConnectionMode::CONNECTION_MODE_SIDEWALK_BLE == this->current_mode)
        {
            api_status = api_processor_cmd_sid_ble_conn_request(this->module);
            delay(5000);
        }
        api_status = api_processor_cmd_sid_send_uplink(this->module, data, len, uplink_type);
    }

    if (API_PROCESSOR_SUCCESS == api_status)
    {
        this->process_received_data();
    }
}

void MCM::set_on_rx_callback(on_rx_callback callback)
{
    this->on_rx_callback_func = callback;
}

void MCM::stop_network()
{
    this->is_joined_network = false;
    do
    {
        if (ConnectionMode::CONNECTION_MODE_NC == this->current_mode)
        {
            break;
        }
        else if (ConnectionMode::CONNECTION_MODE_LORAWAN == this->current_mode)
        {
            // stoppping the lorawan connection
            api_processor_cmd_stop_lorawan_network(this->module);
            // wait for the response
            this->process_received_data();
            break;
        }
        else
        {
            // disconnecting with sidewalk
            api_processor_cmd_sid_stop(this->module);
            // wait for the response
            this->process_received_data();
            break;
        }
    } while (0);
}

mcm_module_hdl_t *MCM::get_module_handle()
{

    return module;
}

HardwareSerial &MCM::get_serial()
{
    return __mcm_serial;
}

void MCM::set_is_rx_received(uint8_t val)
{
    is_rx_received = val;
}

void MCM::set_received_size(uint16_t val)
{
    received_size = val;
}

void MCM::set_is_joined_network(bool val)
{
    is_joined_network = val;
}

void MCM::set_is_last_uplink_pending(bool val)
{
    is_last_uplink_pend = val;
}

uint8_t *MCM::get_downlink_buffer()
{
    return downlink_buffer;
}

on_rx_callback MCM::get_on_rx_callback_func()
{
    return on_rx_callback_func;
}

void MCM::set_last_tx_status(MCM_TX_STATUS status)
{
    this->last_tx_status = status;
}

void MCM::set_serial_rx_timeout(uint32_t timeout)
{
    this->serial_rx_timeout = timeout;
}

bool MCM::is_downlink_available()
{
    return this->is_downlink_avail;
}

void MCM::set_downlink_meta_data(uint16_t len, int8_t rssi, int8_t snr, uint16_t seq_port, bool val)
{
    this->downlink_buffer_size = len;
    this->rssi = rssi;
    this->snr = snr;
    this->seq_port = seq_port;
    this->is_downlink_avail = val;
}

void MCM::get_downlink_data(uint8_t *data, uint16_t *len, int8_t *rssi, int8_t *snr, uint16_t *seq_port)
{
    *len = this->downlink_buffer_size;
    memcpy(data, this->downlink_buffer, *len);
    *rssi = this->rssi;
    *snr = this->snr;
    *seq_port = this->seq_port;
    this->is_downlink_avail = false;
}

bool MCM::get_is_debug_enabled()
{
    return this->is_debug_enabled;
}

void MCM::set_debug_enabled(bool val)
{
    this->is_debug_enabled = val;
}

void MCM::set_context_mgr_is_joined_cmd_received(bool val)
{
    this->_context_mgr_is_joined_cmd_received = val;
}

bool MCM::get_context_mgr_is_joined_cmd_received()
{
    return this->_context_mgr_is_joined_cmd_received;
}

void MCM::set_context_mgr_is_mcm_reset(bool val)
{
    this->_context_mgr_is_mcm_reset = val;
}

bool MCM::get_context_mgr_is_mcm_reset()
{
    bool return_value = false;
    if (this->_context_mgr_is_mcm_reset)
    {
        return_value = true;
        this->_context_mgr_is_mcm_reset = false;
    }
    return return_value;
}

void MCM::hw_reset()
{
    Serial.printf("hw_reset\n");
    digitalWrite(this->_reset_pin, LOW);
    delay(100);
    digitalWrite(this->_reset_pin, HIGH);
    delay(100);

    delay(1000);
    this->process_received_data();
    while (api_processor_get_pending_events(this->module) > 0)
    {
        api_processor_cmd_get_event(this->module); // send the get event command
        this->process_received_data();
    }
}

MCM_STATUS MCM::set_lorawan_class(MCM_LORAWAN_CLASS_TYPE dev_class)
{
    Serial.printf("set_lorawan_class: dev_class=%d\n", (int)dev_class);
    MCM_STATUS status = MCM_STATUS::MCM_ERROR;
    api_processor_status_t api_status = API_PROCESSOR_ERROR;
    do
    {
        if (ConnectionMode::CONNECTION_MODE_LORAWAN != this->current_mode)
        {
            Serial.println("Device is not in the lorawan connection");
            break;
        }
        mrover_lorawan_class_t change_class;

        switch (dev_class)
        {
        case MCM_LORAWAN_CLASS_TYPE::MCM_LRWAN_CLASS_A:
            change_class = MROVER_LORAWAN_CLASS_A;
            break;

        case MCM_LORAWAN_CLASS_TYPE::MCM_LRWAN_CLASS_B:
            change_class = MROVER_LORAWAN_CLASS_B;
            break;

        case MCM_LORAWAN_CLASS_TYPE::MCM_LRWAN_CLASS_C:
            change_class = MROVER_LORAWAN_CLASS_C;
            break;

        default:
            break;
        }

        api_status = api_processor_cmd_set_lorawan_class(this->module, change_class);
        if (API_PROCESSOR_SUCCESS != api_status)
        {
            break;
        }
        // wait for the response
        this->process_received_data();
        status = MCM_STATUS::MCM_OK;

    } while (0);

    return status;
}

bool MCM::get_context_mgr_is_class_change()
{
    Serial.printf("get_context_mgr_is_class_change\n");
    bool return_value = false;
    if (this->_change_class)
    {
        return_value = true;
        this->_change_class = false;
    }
    return return_value;
}

MCM_LORAWAN_CLASS_TYPE MCM::get_lorawan_class()
{
    Serial.printf("get_lorawan_class\n");
    api_processor_cmd_get_lorawan_class(this->module);
    this->process_received_data();
    return this->_dev_class;
}

// TODO: Oxit: at the time of writing this code, the file transfer is only for
// fw update for host side
//  in future we will use this function for other file transfer
//  In that case, just use the callback function to get the file data
MCM_STATUS MCM::start_file_transfer(ver_type_1_t version)
{
    Serial.printf("start_file_transfer: version=%d.%d.%d\n", version.major, version.minor, version.patch);
    MCM_STATUS status = MCM_STATUS::MCM_ERROR;
    api_processor_status_t api_status = API_PROCESSOR_ERROR;

    do
    {
        // Send the start file transfer command
        api_status = api_processor_cmd_start_file_transfer(this->module, version);
        if (API_PROCESSOR_SUCCESS != api_status)
        {
            break;
        }

        // Wait for the response
        this->process_received_data();
        status = MCM_STATUS::MCM_OK;

    } while (0);

    return status;
}

MCM_STATUS MCM::get_segmented_file_download_status(get_seg_file_status_t *seg_status)
{
    Serial.printf("get_segmented_file_download_status: seg_status=%p\n", seg_status);
    MCM_STATUS status = MCM_STATUS::MCM_ERROR;
    api_processor_status_t api_status = API_PROCESSOR_ERROR;
    memset(seg_status, 0, sizeof(get_seg_file_status_t));
    do
    {
        api_status = api_processor_cmd_get_seg_file_transfer_status(this->module);
        if (API_PROCESSOR_SUCCESS != api_status)
        {
            break;
        }

        // Wait for the response
        this->process_received_data();
        status = MCM_STATUS::MCM_OK;

    } while (0);

    memcpy(seg_status, &this->seg_file_status, sizeof(get_seg_file_status_t));
    return status;
}

MCM_STATUS MCM::trigger_firmware_update(ver_type_1_t version)
{
    Serial.printf("trigger_firmware_update: version=%d.%d.%d\n", version.major, version.minor, version.patch);
    MCM_STATUS status = MCM_STATUS::MCM_ERROR;
    api_processor_status_t api_status = API_PROCESSOR_ERROR;
    do
    {
        api_status = api_processor_cmd_trigger_fw_update(this->module, version);
        if (API_PROCESSOR_SUCCESS != api_status)
        {
            break;
        }

        // Wait for the response
        this->process_received_data();
        status = MCM_STATUS::MCM_OK;

    } while (0);

    return status;
}

MCM_STATUS MCM::factory_reset()
{
    Serial.printf("factory_reset\n");
    MCM_STATUS status = MCM_STATUS::MCM_ERROR;
    api_processor_status_t api_status = API_PROCESSOR_ERROR;
    do
    {
        api_status = api_processor_cmd_factory_reset(this->module);
        if (API_PROCESSOR_SUCCESS != api_status)
        {
            break;
        }

        // Wait for the response
        this->process_received_data();
        status = MCM_STATUS::MCM_OK;

    } while (0);

    return status;
}

bool MCM::is_new_firmware()
{
    bool return_value = false;
    if (this->is_new_firmware_downloaded)
    {
        return_value = true;
        this->is_new_firmware_downloaded = false;
    }
    return return_value;
}

void MCM::set_host_app_version(ver_type_1_t host_version)
{
    Serial.printf("set_host_app_version: host_version=%d.%d.%d\n", host_version.major, host_version.minor, host_version.patch);
    this->host_version = host_version;
}

void MCM::retrieveLibraryVersions(ver_type_1_t *mcm_rover_lib_ver, ver_type_1_t *c_lib_ver)
{
    Serial.printf("retrieveLibraryVersions: mcm_rover_lib_ver=%p, c_lib_ver=%p\n", mcm_rover_lib_ver, c_lib_ver);
    mcm_rover_lib_ver->major = MCM_ROVER_LIB_VER_MAJOR;
    mcm_rover_lib_ver->minor = MCM_ROVER_LIB_VER_MINOR;
    mcm_rover_lib_ver->patch = MCM_ROVER_LIB_VER_PATCH;
    api_processor_get_lib_ver(c_lib_ver);
}

MCM_STATUS MCM::process_fw_update()
{
    Serial.printf("process_fw_update\n");
    if(this->seg_file_status.cmd_type.bin_type == FUOTA_BINARY_TYPE_MCM)
    {
         Serial.println("MCM firmware present - triggering MCM firmware update");
         return this->trigger_firmware_update(seg_file_status.fw_ver);
    }
    else
    {
        Serial.println("Host firmware present - triggering host firmware update");
        return this->start_file_transfer(seg_file_status.fw_ver);
    }
}