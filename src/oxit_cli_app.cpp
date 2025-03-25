/**
 * @file    command_line_application.c
 * @author  OXIT embedded firmware team
 * @brief   This source file defines the command line application APIs
 * @version 0.1
 * @date    2023-03-02
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
/******************************************************************************/
/* Include Files */
/******************************************************************************/
/**<sys includes*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>

/**<Application includes*/
#include "hal_error_codes.h"
#include "oxit_cli.h"
#include "oxit_cli_app.h"
#include <oxit_nvs.h>
#include "lrwan_sidewalk_ex.h"


#define CLI_APP_NAME "oxit_cli"
/******************************************************************************/
/* Scope Limited variables */
/******************************************************************************/
static char cli_buffer[128] = {0};

static bool disable_cli_input = false;

/******************************************************************************/
/* Static Function Declaration */
/******************************************************************************/
/**
 * @brief Sets the device EUI.
 *
 * @param pu8_input_value The input value for device EUI.
 * @param pfun_uart_tx Function to send bytes over UART.
 * @return int Return status code.
 */
static int set_deveui_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx);

/**
 * @brief Sets the join EUI.
 *
 * @param pu8_input_value The input value for join EUI.
 * @param pfun_uart_tx Function to send bytes over UART.
 * @return int Return status code.
 */
static int set_join_eui_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx);

/**
 * @brief Sets the application key.
 *
 * @param pu8_input_value The input value for application key.
 * @param pfun_uart_tx Function to send bytes over UART.
 * @return int Return status code.
 */
static int set_app_key_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx);

/**
 * @brief Erases the stored credentials.
 *
 * @param pu8_input_value The input value.
 * @param pfun_uart_tx Function to send bytes over UART.
 * @return int Return status code.
 */
static int erase_credentials_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx);

/**
 * @brief Reboots the device.
 *
 * @param pu8_input_value The input value.
 * @param pfun_uart_tx Function to send bytes over UART.
 * @return int Return status code.
 */
static int reboot_device_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx);

/**
 * @brief Sends an uplink message immediately.
 *
 * @param pu8_input_value The input value.
 * @param pfun_uart_tx Function to send bytes over UART.
 * @return int Return status code.
 */
static int send_uplink_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx);

/**
 * @brief Switches the protocol mode.
 *
 * @param pu8_input_value The input value.
 * @param pfun_uart_tx Function to send bytes over UART.
 * @return int Return status code.
 */
static int protocol_switch_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx);

/**
 * @brief Sends the fw update request.
 *
 * @param pu8_input_value The input value.
 * @param pfun_uart_tx A function to send bytes over uart.
 *
 * @return int Return value.
 *
 * @throws None
 */
static int send_fw_update_request_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx);

/**
 * @brief cli_send_bytes call back to send the bytes
 *
 * @return true   If receive data not available or error occured while
 * processing
 * @return false  If success
 */
hal_error_code_t cli_send_bytes(const uint8_t *p_u8_tx_bytes, size_t len);
/**
 * @brief cli_receive_byte call back to receive the bytes
 *
 * @return true   If receive data not available or error occured while
 * processing
 * @return false  If success
 */
hal_error_code_t cli_receive_byte(uint8_t *p_u8_rx_byte);

void helper_print_hex_array(const uint8_t* arr, size_t len);

/******************************************************************************/
/* Global Variable Definition */
/******************************************************************************/
cli_config_t cli_cmds_cfg = {
     '\r',
     " ",
     "?",
     STARTUP_MSG,
     true,
     false,

};

cli_handle_t cli_hdl = {
    cli_buffer,
    128,
    &cli_cmds_cfg,
    cli_receive_byte,
    cli_send_bytes,
    false,
    0,
    { NULL,NULL},

};
//this function definition is in the main ino file
void send_uplink_now();

void send_fw_update_request();

/******************************************************************************/
/* enter_bootloader application variable */
/******************************************************************************/
cli_command_t cli_commands[] = {{
                                                "deveui",
                                                CLI_APP_NAME" deveui 001122334455667788 <enter>",
                                                "To set lorawan deveui",
                                                set_deveui_callback,
                                            },
                                            {
                                                 "joineui",
                                                 CLI_APP_NAME" joineui 001122334455667788 <enter>",
                                                 "To set lorawan joineui",
                                                 set_join_eui_callback,
                                            },
                                            {
                                                 "appkey",
                                                 CLI_APP_NAME" appkey 00112233445566778899aabbccddeeff <enter>",
                                                 "To set lorawan appkey",
                                                 set_app_key_callback,
                                            },
                                            {
                                                 "reboot",
                                                 CLI_APP_NAME" reboot <enter>",
                                                 "To reboot the device",
                                                 reboot_device_callback,
                                            },
                                            {
                                                 "erase",
                                                 CLI_APP_NAME" erase <enter>",
                                                 "To erase stored credentials",
                                                 erase_credentials_callback,
                                            },
                                            {
                                                 "uplink_now",
                                                 CLI_APP_NAME" send_uplink <enter>",
                                                 "To send uplink",
                                                 send_uplink_callback,
                                            },
                                            {
                                                "fw_update",
                                                CLI_APP_NAME" Send the firmware update request <enter>",
                                                "To send the firmware update request",
                                                send_fw_update_request_callback,
                                            },
                                            {
                                                "protocol_switch",
                                                CLI_APP_NAME" protocol_switch <mode>",
                                                "Switch protocol mode. Modes: lorawan, sw_ble, sw_fsk, sw_css",
                                                protocol_switch_callback,
                                            },

                                            };

cli_app_t register_app = {  CLI_APP_NAME, 
                                        "1.0.0",
                                        cli_commands, 
                                        sizeof(cli_commands)/sizeof(cli_command_t), 
                                        };

/******************************************************************************/
/* Function Definition */
/******************************************************************************/
hal_error_code_t cli_receive_byte(uint8_t *pu8_rx_byte)
{
    static uint8_t u8_byte   = 0;
    hal_error_code_t enu_err = HAL_ERROR_OUT_OF_BOUND;


    u8_byte = Serial.read();

    if ((u8_byte >= 0) && (u8_byte < 255))
    {
        *pu8_rx_byte = u8_byte;
        enu_err      = HAL_ERROR_OK;
    }
    return enu_err;
}
hal_error_code_t cli_send_bytes(const uint8_t *p_u8_tx_bytes, size_t len)
{
    char u8_print_char = 0;

    for (uint32_t u32_index = 0; u32_index <len; u32_index++)
    {
        u8_print_char = (char )*p_u8_tx_bytes;
        if (u8_print_char == CR)
        {
            Serial.print(LF);
        }
        Serial.print(u8_print_char);

        p_u8_tx_bytes++;
    }
    return HAL_ERROR_OK;
}
#pragma optimize("", off)
void convert_string_hex(const uint8_t *s_buf, uint8_t s_len, uint8_t *h_buf, uint8_t h_len)
{   
    // helper_print_hex_array(s_buf, s_len);
     char s_val[2] = {0};
     char *endptr = NULL;
     volatile uint8_t converted = 0;
    for(uint8_t i = 0; i < h_len;i++)
    {   
        strncpy(s_val, (const char*)&s_buf[2 * i], 2);
        
        // if (sscanf(s_val, "%02x", &converted) != 1)
        // {   
        //     Serial.println("Error converting string to hex");
        //     break;
        // }
        h_buf[i] = strtol(s_val, &endptr, 16);

        
    }
//    helper_print_hex_array(h_buf, h_len);
   Serial.println("");
}   
#pragma optimize("", on)

#pragma optimize("", off)
bool validate_hex_conversion(const uint8_t *buf, uint8_t len)
{
    bool return_value = true;
    for(uint8_t i = 0; i < len; i++)
    {
        if(!isxdigit(buf[i]))
        {
            return_value = false;
            break;
        }
    }
    return return_value;
}
#pragma optimize("", on)

#pragma optimize("", off)
static int set_deveui_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx)
{
    //perform the deveui validation
    do
    {  
        // Serial.println("Received data len : " + String(strlen(pu8_input_value)));
        if(strlen(pu8_input_value) != 16)
        {
            Serial.println("Invalid deveui length, Please try again");
            break;
        }

        if(validate_hex_conversion((const uint8_t*)pu8_input_value, 16) == false)
        {
            Serial.println("Invalid deveui hex value, Please try again");
            break;
        }
        uint8_t deveui[8];
       memset(deveui, 0, sizeof(deveui));
        // convert string to hex
        convert_string_hex((const uint8_t*)pu8_input_value, 16, deveui, sizeof(deveui));

        volatile bool ret = nvs_storage_set_dev_eui(deveui);

        if(false == ret)
        {
            Serial.println("Failed to store deveui, Please try again");
            break;
        }
        // helper_print_hex_array(deveui, sizeof(deveui));  
        Serial.println("Deveui set successfully");
        Serial.println("Please reboot the device manually if all credentials dev eui and join eui and app key are set successfully");

    } while (0);

    return 1;
}
#pragma optimize("", on)

#pragma optimize("", off)
static int set_join_eui_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx)
{
     //perform the deveui validation
    do
    {  
        if(strlen(pu8_input_value) != 16)
        {
            Serial.println("Invalid join eui length, Please try again");
            break;
        }

        if(validate_hex_conversion((const uint8_t*)pu8_input_value, 16) == false)
        {
            Serial.println("Invalid join eui hex value, Please try again");
            break;
        }
         uint8_t joineui[8];
       memset(joineui, 0, sizeof(joineui));
        // convert string to hex
        convert_string_hex((const uint8_t*)pu8_input_value, 16, joineui, sizeof(joineui));

        if(false == nvs_storage_set_join_eui(joineui))
        {
            Serial.println("Failed to store join eui, Please try again");
            break;
        }
       
        
        Serial.println("joineui set successfully");
        Serial.println("Please reboot the device manually if all credentials dev eui and join eui and app key are set successfully");

    } while (0);

    return 1;
}
#pragma optimize("", on)

#pragma optimize("", off)
static int set_app_key_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx)
{
     //perform the deveui validation
    do
    {  
        if(strlen(pu8_input_value) != 32)
        {
            Serial.println("Invalid app key length, Please try again");
            break;
        }

        if(validate_hex_conversion((const uint8_t*)pu8_input_value, 32) == false)
        {
            Serial.println("Invalid appkey hex value, Please try again");
            break;
        }
        uint8_t app_key[16];
       memset(app_key, 0, sizeof(app_key));
        // convert string to hex
        convert_string_hex((const uint8_t*)pu8_input_value, 32, app_key, sizeof(app_key));

        if(false == nvs_storage_set_app_key(app_key))
        {
            Serial.println("Failed to store app key, Please try again");
            break;
        }
       
       
        Serial.println("app key set successfully");
        Serial.println("Please reboot the device manually if all credentials dev eui and join eui and app key are set successfully");

    } while (0);

    return 1;
}
#pragma optimize("", on)

/**
 * @brief Initializes the command line interface by providing application
 *        specific commands list, maximum number of commands available,
 *        rx callback function and tx callback function.
 *
 */
void init_command_line_app(void)
{
    hal_error_code_t enu_error = HAL_ERROR_OK;

    do
    {
        enu_error = cli_init(&cli_hdl, &cli_cmds_cfg);
        
        enu_error = cli_register_new_app(&cli_hdl, &register_app);

    } while (0);
}

/**
 * @brief  Wraps the comand line process function from the CLI module
 *
 * @return true   If receive data not available or error occured while
 * processing
 * @return false  If success
 */
bool process_command_line_app(void)
{
    if (false == disable_cli_input)
    {
        return cli_process(&cli_hdl);
    }
    else
    {
        return false;
    }
}

static int erase_credentials_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx)
{   
    if(false == nvs_storage_erase())
    {
        Serial.println("Failed to erase credentials, Please try again");
        return 1;
    }
    Serial.println("Credentials erased successfully, Please reboot device and then enter credentials manually");
    return 1;
}

static int reboot_device_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx)
{
    Serial.println("Rebooting device in 3 seconds");
    delay(3000);
    ESP.restart();
    return 1;
}

static int send_uplink_callback(const char *pu8_input_value,cli_send_bytes_t pfun_uart_tx)
{
    send_uplink_now();
    return 1;
}

static int send_fw_update_request_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx)
{
    send_fw_update_request();
    return 1;
}

static int protocol_switch_callback(const char *pu8_input_value, cli_send_bytes_t pfun_uart_tx)
{
    // Check if user supplied a mode string
    if (pu8_input_value == NULL || strlen(pu8_input_value) == 0)
    {
        Serial.println("Usage: protocol_switch <mode>");
        Serial.println("Available modes: lorawan, sidewalk_ble, sidewalk_fsk, sidewalk_css");
        return 1;
    }

    // Copy and convert the parameter to lower-case for simple comparison
    char mode_param[32] = {0};
    strncpy(mode_param, pu8_input_value, sizeof(mode_param) - 1);
    for (int i = 0; mode_param[i]; i++)
    {
        mode_param[i] = tolower(mode_param[i]);
    }

    // Parse the parameter and prepare the corresponding ConnectionMode value
    ConnectionMode new_mode;
    if (strcmp(mode_param, "lorawan") == 0)
    {
        new_mode = ConnectionMode::CONNECTION_MODE_LORAWAN;
    }
    else if (strcmp(mode_param, "sw_ble") == 0)
    {
        new_mode = ConnectionMode::CONNECTION_MODE_SIDEWALK_BLE;
    }
    else if (strcmp(mode_param, "sw_fsk") == 0)
    {
        new_mode = ConnectionMode::CONNECTION_MODE_SIDEWALK_FSK;
    }
    else if (strcmp(mode_param, "sw_css") == 0)
    {
        new_mode = ConnectionMode::CONNECTION_MODE_SIDEWALK_CSS;
    }
    else
    {
        Serial.println("Invalid protocol mode specified.");
        Serial.println("Available modes: lorawan, sw_ble, sw_fsk, sw_css");
        return 1;
    }

    // Call the new protocol switch API to update the mode
    switch_protocol_mode(new_mode);
    return 1;
}
