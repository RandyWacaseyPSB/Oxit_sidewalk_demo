/**
 * @file    command_line_interface.h
 * @author  OXIT embedded firmware team
 * @brief   This header file defines the data structures and API function
 * prototypes of a portable, reusable command line interface which is portable
 * and reusable in C language based firmware projects.
 * @version 0.1
 * @date    2022-06-02
 *
 * @copyright Copyright © 2022 Oxit, LLC.
 *            All Rights Reserved.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OXTECH_MIDDLEWARE_OXIT_CLI_OXIT_CLI_H_
#define OXTECH_MIDDLEWARE_OXIT_CLI_OXIT_CLI_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hal_error_codes.h"
#include "linked_list.h"

#define DELETE          0x7F
#define BACKSPACE       0x08
#define CARRIAGE_RETURN 13u
#define TAB_SPACE       "\t"
#define NULL_CHARACTER  '\0'

    typedef hal_error_code_t (*cli_send_bytes_t)(const uint8_t *p_u8_tx_bytes, size_t len);  /**< Callback app function used to send the bytes          */
    typedef hal_error_code_t (*cli_receive_byte_t)(uint8_t *p_u8_rx_byte);                   /**< Callback app function used to receive the bytes */
    typedef int (*command_handler_t)(const char *p_input_value, cli_send_bytes_t p_func_tx); /**< Callback app function to handle the CLI */

    /**
     * Struct to specify the attributes of a given cli command
     */
    typedef struct
    {
        const char *const p_command_string;      /**< Command string, the actual string to
                                                    execute the given command         */
        const char *const p_command_usage;       /**< Usage string that will describe how
                                                    the command should be used         */
        const char *const p_command_description; /**< Description string to tell the user
                                                    what the command acutally does     */
        command_handler_t command_handler;       /**< Callback function to be executed when
                                                    the command is matched by module */
    } cli_command_t;

    /**
     * Struct to specify the attributes of a given cli application
     */
    typedef struct
    {
        const char *const p_app_name; /**< Application string, the actual string to
                                         execute the given application */
        const char *const version;    /**< Application version string    */
        cli_command_t *p_commands;    /**< Pointer to an array of commands    */
        size_t nbr_of_cmds;           /**< The number of commands in the p_commands array */
    } cli_app_t;

    /**
     * Struct that holds the configuration of the cli instance.
     * To be used with a CLI handle.
     */
    typedef struct
    {
        const char endline;        /**< Which character denotes the end of a full command to
                                      start parsing, typically \r or \n */
        const char *arg_delimeter; /**< The delimeter string that separates application name,
                                      command name and passed arguments. Typically space  */
        const char *help_cmd;      /**< The argument that is used to print the help menu,
                                      for both application and commands */
        const char *startup_msg;   /**< Startup message is printed the first time the
                                      cli module is ran                     */
        const bool echo_enable;    /**< All received bytes will be echoed back if this is
                                      true                              */
        const bool case_sensitive; /**< Should the CLI care about case */

    } cli_config_t;

    typedef struct
    {
        char *cmd_buffer;              /**< Buffer to hold the startup message for this CLI
                                          instance */
        const size_t max_cmd_size;     /**< Maximum size of a command, used to manage the command
                                          buffer. Typically it is the buffer size -1 */
        cli_config_t *config;          /**< Pointer to configuration struct */
        cli_receive_byte_t rx_byte_cb; /**< Function pointer, a function used to
                                          receive one byte at a time */
        cli_send_bytes_t tx_bytes_cb;  /**< Function pointer, a function used to send bytes */

        bool _startup_msg_done;  /**< Flag used internally to check if startup message
                                    is printed for this CLI instance */
        uint32_t _rx_idx;        /**< Flag used internally to count the number of bytes
                                    received */
        linked_list_t _app_list; /**< Linked list of the applications, initialized internally */
    } cli_handle_t;

    /**
     * @brief Initialize the command line interface
     * @param p_str_cli_hdl Pointer to the cli module handle
     * @param p_str_cfg Pointer to filled configuration struct
     * @return HAL_ERROR_OK if success and HAL_ERROR_MEMORY_NULL if pointer is NULL
     */
    hal_error_code_t cli_init(cli_handle_t *const p_str_cli_hdl, cli_config_t *p_str_cfg);

    /**
     * @brief Process command line inputs, must be called periodically or on data
     * received
     * @param p_str_cli_hdl Pointer to the cli module handle
     * @return HAL_ERROR_OK if success HAL_ERROR_MEMORY_NULL if pointer is NULL and
     * HAL_ERROR_OUT_OF_BOUND when byte received is not ASCII
     */
    hal_error_code_t cli_process(cli_handle_t *const p_str_cli_hdl);

    /**
     * @brief Register a new app to the linked list
     * @param p_str_cli_hdl Pointer to the cli module handle
     * @param p_app Application node to be added to the linked list
     * @return HAL_ERROR_OK if success and HAL_ERROR_MEMORY_NULL if pointer is NULL
     */
    hal_error_code_t cli_register_new_app(cli_handle_t *const p_str_cli_hdl, cli_app_t *p_app);

    /**
     * @brief Unregister a new app to the linked list
     * @param p_str_cli_hdl Pointer to the cli module handle
     * @param p_app Application node to be removed to the linked list
     * @return HAL_ERROR_OK if success and HAL_ERROR_MEMORY_NULL if pointer is NULL
     */
    hal_error_code_t cli_unregister_new_app(cli_handle_t *const p_str_cli_hdl,
                                            cli_app_t *p_app); // TODO Implement the function

#ifdef __cplusplus
}
#endif

#endif /* OXTECH_MIDDLEWARE_OXIT_CLI_OXIT_CLI_H_ */

/************************ (C) Copyright © 2022 Oxit, LLC *****END OF FILE*****/
