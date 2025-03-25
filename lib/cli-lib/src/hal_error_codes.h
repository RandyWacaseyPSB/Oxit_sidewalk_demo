/**
 * @file hal_error_codes.h
 * @author Hisham D.
 * @date May 31, 2021
 * @brief Brief File Description
 *
 * Long file description
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

#ifndef OXIT_SUBMOD_HAL_HAL_HAL_ERROR_CODES_H_
#define OXIT_SUBMOD_HAL_HAL_HAL_ERROR_CODES_H_

/**
 * @brief Enumeration of error codes used in the Hardware Abstraction Layer (HAL).
 *
 * This enumeration lists various error codes that can be returned by the HAL functions.
 * Each code represents a specific error condition or status.
 */
typedef enum error_codes
{
    HAL_ERROR_OK = 0x00, /**< Operation completed successfully. */

    HAL_ERROR_NOT_INITIALIZED   = 0x01, /**< Module or peripheral not initialized. */
    HAL_ERROR_NOT_READY         = 0x02, /**< Module or peripheral not ready. */
    HAL_ERROR_NOT_FOUND         = 0x03, /**< Item or resource not found. */
    HAL_ERROR_OUT_OF_BOUND      = 0x04, /**< Index or parameter out of bounds. */
    HAL_ERROR_INVALID_ARGUMENTS = 0x05, /**< Invalid input or arguments. */
    HAL_ERROR_BAD_FORMAT        = 0x06, /**< Incorrect data format or structure. */

    HAL_ERROR_PERIPH_UART_FAIL = 0x100, /**< UART peripheral operation failure. */
    HAL_ERROR_PERIPH_I2C_FAIL  = 0x101, /**< I2C peripheral operation failure. */
    HAL_ERROR_PERIPH_SPI_FAIL  = 0x102, /**< SPI peripheral operation failure. */
    HAL_ERROR_PERIPH_ADC_FAIL  = 0x103, /**< ADC peripheral operation failure. */

    HAL_ERROR_TIMING_TIMEOUT = 0x100, /**< Timing-related operation timeout. */

    HAL_ERROR_DRIVER_GENERAL = 0x200, /**< General driver-related error. */

    HAL_ERROR_DATA_CRC = 0x300, /**< Data CRC (Cyclic Redundancy Check) error. */

    HAL_ERROR_MEMORY_NULL = 0x400, /**< Memory-related null pointer error. */

} hal_error_code_t;


#endif /* OXIT_SUBMOD_HAL_HAL_HAL_ERROR_CODES_H_ */
