/**
 * @file frame_parse.h
 * @author Ankit Bansal (ankit.bansal@oxit.com)
 * @brief Header file for the frame parser. 
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


#ifndef __FRAME_PARSE_H__
#define __FRAME_PARSE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
 * INCLUDES
 **********************************************************************************************************/
#include "commands_defs.h"
#include <stdbool.h>

/**********************************************************************************************************
 * MACROS AND DEFINES
 **********************************************************************************************************/

/**********************************************************************************************************
 * TYPEDEFS
 **********************************************************************************************************/
typedef enum {

    FP_SUCCESS = 0,
    FP_INVALID_PARAMETERS,
    FP_INVALID_RETURN_CODE,
    FP_INVALID_COMMAND_TYPE,
    FP_INVALID_COMMAND_CODE,
    FP_INVALID_CRC,
    FP_ERROR

}fp_api_status_t;

/**********************************************************************************************************
 * EXPORTED VARIABLES
 **********************************************************************************************************/

/**********************************************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 **********************************************************************************************************/
/**
 * @brief This function appends the CRC to the data in the buffer.
 *
 * @param[in,out] data Pointer to the buffer containing the data and CRC.
 * @param[in] len Length of the data in the buffer.
 *
 * @return Returns the status of the CRC append operation.
 */
fp_api_status_t fp_append_crc(uint8_t *data,uint16_t len);

/**
 * @brief Check if the frame is valid
 *
 * This function checks if the frame is valid by checking for
 * the return code, command type and command code and the CRC
 *
 * @param[in] data Pointer to the frame data
 * @param[in] len Length of the frame data
 *
 * @retval FP_SUCCESS The frame is valid
 * @retval FP_INVALID_PARAMETERS Invalid data pointer or length
 * @retval FP_INVALID_RETURN_CODE Invalid return code in the frame
 * @retval FP_INVALID_COMMAND_TYPE Invalid command type in the frame
 * @retval FP_INVALID_COMMAND_CODE Invalid command code in the frame
 * @retval FP_INVALID_CRC Invalid CRC in the frame
 * @retval FP_ERROR Generic error
 *
 */
fp_api_status_t fp_is_valid_response_frame(uint8_t *data, uint16_t len);

/**
 * @brief Check if the frame is notification or response
 *
 *
 * @param[in] data Pointer to the frame data
 * @param[in] len Length of the frame data
 *
 * @retval true The frame is notification
 * @retval false The frame is response
 *
 */
bool fp_is_frame_notification(uint8_t *data, uint16_t len);

/**
 * @brief Check if the frame is a valid notify frame
 *
 * This function checks if the frame is valid notify frame by checking for
 * the return code and the notification code.
 *
 * @param[in] data Pointer to the frame data
 * @param[in] len Length of the frame data
 *
 * @retval FP_SUCCESS The frame is valid notify frame
 * @retval FP_INVALID_PARAMETERS Invalid data pointer or length
 * @retval FP_INVALID_RETURN_CODE Invalid return code in the frame
 * @retval FP_INVALID_COMMAND_CODE Invalid command code in the frame
 * @retval FP_ERROR Generic error
 *
 */
fp_api_status_t fp_is_valid_notify_frame(uint8_t *data, uint16_t len);

/**
 * @brief This function returns the number of pending events.
 *
 * @param[in] data Pointer to the notification frame received.
 * @param[in] len Length of the notification frame.
 *
 * @return The number of pending events.
 */
uint8_t fp_get_pending_event_count(uint8_t *data, uint16_t len);

/**
 * @brief Check if the frame is a single frame.
 *
 * This function checks if the frame is a single frame by checking if the first
 * byte is MROVER_RC_NOTIFY_EVENTS and the length is MIN_RX_PAYLOAD_LEN.
 *
 * @param[in] p_data Pointer to the frame data.
 * @param[in] u16_len Length of the frame.
 *
 * @retval true The frame is a single frame.
 * @retval false The frame is not a single frame.
 */
bool fp_is_single_frame(uint8_t *p_data,uint16_t u16_len);



#ifdef __cplusplus
}
#endif




#endif // __FRAME_PARSE_H__