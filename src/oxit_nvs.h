/**
 * @file oxit_nvs.h
 * @author Ankit Bansal (ankit.bansal@oxit.com)
 * @brief Declarations for non-volatile storage APIs
 * @version 0.1
 * @date 2024-06-19
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

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
 * INCLUDES
 **********************************************************************************************************/
#include <stdint.h>

/**********************************************************************************************************
 * MACROS AND DEFINES
 **********************************************************************************************************/

/**********************************************************************************************************
 * TYPEDEFS
 **********************************************************************************************************/

/**********************************************************************************************************
 * EXPORTED VARIABLES
 **********************************************************************************************************/

/**********************************************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 **********************************************************************************************************/

/**
 * @brief Initializes the NVS (Non-Volatile Storage) module.
 *
 * @return true if initialization is successful, false otherwise.
 */
bool nvs_storage_init();


/**
 * @brief Retrieves the reboot count from the NVS (Non-Volatile Storage) module.
 *
 * @return The reboot count. If an error occurs, returns 0.
 */
uint16_t nvs_storage_get_reboot_count();

/**
 * @brief Retrieves the LoRaWAN credentials from the NVS (Non-Volatile Storage) module.
 *
 * @param app_key Pointer to the buffer to store the app key.
 * @param join_eui Pointer to the buffer to store the join EUI.
 * @param dev_eui Pointer to the buffer to store the dev EUI.
 *
 * @return true if the credentials are successfully retrieved, false otherwise.
 */
bool nvs_storage_get_lorawan_cred(uint8_t *app_key, uint8_t *join_eui, uint8_t *dev_eui);


/**
 * @brief Stores the LoRaWAN app key in the NVS (Non-Volatile Storage) module.
 *
 * @param app_key Pointer to the buffer containing the app key.
 *
 * @return true if the app key is successfully stored, false otherwise.
 */
bool nvs_storage_set_app_key(uint8_t *app_key);

/**
 * @brief Stores the LoRaWAN join EUI in the NVS (Non-Volatile Storage) module.
 *
 * @param join_eui Pointer to the buffer containing the join EUI.
 *
 * @return true if the join EUI is successfully stored, false otherwise.
 */
bool nvs_storage_set_join_eui(uint8_t *join_eui);

/**
 * @brief Stores the LoRaWAN dev EUI in the NVS (Non-Volatile Storage) module.
 *
 * @param dev_eui Pointer to the buffer containing the dev EUI.
 *
 * @return true if the dev EUI is successfully stored, false otherwise.
 */
bool nvs_storage_set_dev_eui(uint8_t *dev_eui);

/**
 * @brief Erases all data stored in the NVS (Non-Volatile Storage) module.
 *
 * @return true if the erase operation is successful, false otherwise.
 */
bool nvs_storage_erase();

#ifdef __cplusplus
}
#endif