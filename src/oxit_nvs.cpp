/**
 * @file oxit_nvs.h
 * @author Ankit Bansal (ankit.bansal@oxit.com)
 * @brief Implmentation for non-volatile storage APIs
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

/******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <Arduino.h>
#include "oxit_nvs.h"
#include "SparkFun_External_EEPROM.h" 
/******************************************************************************
 * USING NAMESPACES
 ******************************************************************************/


/******************************************************************************
 * EXTERN VARIABLES
 ******************************************************************************/

/******************************************************************************
 * PRIVATE MACROS AND DEFINES
 ******************************************************************************/

#define USE_INTERNAL_FLASH 1

#define STORAGE_NAMESPACE "storage"
#define DEVEUI_KEY "deveui"
#define JOIN_EUI_KEY "join_eui"
#define APP_KEY_KEY "app_key"
#define REBOOT_COUNT_KEY "reboot_count" 

#define REBOOT_LOC 0
#define DEVEUI_LOC 8
#define JOIN_EUI_LOC 24
#define APP_KEY_LOC 40

/******************************************************************************
 * PRIVATE TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 * STATIC VARIABLES
 ******************************************************************************/
ExternalEEPROM myMem;
static bool is_nvs_init = false;
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
bool is_all_ff(uint8_t* arr, uint8_t len);

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
bool nvs_storage_init()
{
#if USE_INTERNAL_FLASH
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if(ESP_OK != ret)
    {
        return false;
    }

    return true;
#else
    myMem.setMemoryType(512);
    is_nvs_init = myMem.begin();
    return is_nvs_init;
    
#endif 
}

uint16_t nvs_storage_get_reboot_count()
{
    uint16_t reboot_count = 0;
    #if USE_INTERNAL_FLASH
    nvs_handle_t handle;

    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &handle);
    do
    {
        if (err != ESP_OK) 
        {   
            Serial.println("Failed to open NVS");
            break;
        }

        err = nvs_get_u16(handle, REBOOT_COUNT_KEY, &reboot_count);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
        {
            Serial.println("Failed to read reboot count");
            break;
        }

        Serial.println("Updating restart counter in NVS ... ");
        reboot_count = reboot_count + 1;
        err = nvs_set_u16(handle, "reboot_count", reboot_count);
        if (err != ESP_OK)
        {
            Serial.println("Failed to update reboot count");
            break;
        }
        err = nvs_commit(handle);
        if (err != ESP_OK)
        {
            Serial.println("Failed to commit updated reboot count");
            break;
        }
    } while (0);
    
    nvs_close(handle);
#else

    if (false == is_nvs_init)
    {
        return 0;
    }
    myMem.get(0, reboot_count);
    reboot_count = reboot_count + 1;
    myMem.put(0, reboot_count);
#endif
    return reboot_count;
}


bool nvs_storage_get_dev_eui(uint8_t *devui)
{   
    bool return_value = false;
#if USE_INTERNAL_FLASH
    nvs_handle_t storage_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &storage_handle);
    do
    {
        if (err != ESP_OK) 
        {
            Serial.println("Failed to open NVS");
            break;
        }
        size_t required_size = 8;
        err = nvs_get_blob(storage_handle,DEVEUI_KEY, devui, &required_size);
       
        if (err != ESP_OK) 
        {
            Serial.println("Failed to read dev_eui");
            break;
        }
        return_value = true;
    } while (0);
    
    nvs_close(storage_handle);
#else
    if(false == is_nvs_init)
    {
        return false;
    }

    if (0 == myMem.read(DEVEUI_LOC, devui, 8))
    {
        if(is_all_ff(devui, 8))
        {
            return_value = false;
        }
        else
        {
            return_value = true;
        }
    }
    else
    {
        return_value = false;
    }
#endif
    return return_value;
}

bool nvs_storage_set_dev_eui(uint8_t *devui)
{   
    bool return_value = false;
#if USE_INTERNAL_FLASH
    nvs_handle_t storage_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &storage_handle);
    do
    {
        if (err != ESP_OK)
        {
            Serial.println("Failed to open NVS");
            break;
        }
        err = nvs_set_blob(storage_handle, DEVEUI_KEY, devui, 8);
        if (err != ESP_OK)
        {
            Serial.println("Failed to write dev_eui");
            break;
        }
       
        err = nvs_commit(storage_handle);
        if (err != ESP_OK)
        {
            Serial.println("Failed to commit updated dev_eui");
            break;
        }
        return_value = true;
    } while (0);

    nvs_close(storage_handle);
#else
    if(false == is_nvs_init)
    {
        return false;
    }

    if (0 == myMem.write(DEVEUI_LOC, devui, 8))
    {
        return_value = true;
    }
    else
    {
        return_value = false;
    }
#endif
    return return_value;
}

bool nvs_storage_get_join_eui(uint8_t *join_eui)
{
    bool return_value = false;
#if USE_INTERNAL_FLASH
    nvs_handle_t storage_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &storage_handle);
    do
    {
        if (err != ESP_OK)
        {
            Serial.println("Failed to open NVS");
            break;
        }
        size_t required_size = 8;
        err = nvs_get_blob(storage_handle, JOIN_EUI_KEY, join_eui, &required_size);
        
        if (err != ESP_OK)
        {
            Serial.println("Failed to read join_eui");
            break;
        }
        return_value = true;
    } while (0);

    nvs_close(storage_handle);
#else
    if(false == is_nvs_init)
    {
        return false;
    }
    if (0 == myMem.read(JOIN_EUI_LOC, join_eui, 8))
    {
         if(is_all_ff(join_eui, 8))
        {
            return_value = false;
        }
        else
        {
            return_value = true;
        }
    }
    else
    {
        return_value = false;
    }
#endif
    return return_value;
}


bool nvs_storage_set_join_eui(uint8_t *join_eui)
{
    bool return_value = false;
#if USE_INTERNAL_FLASH
    nvs_handle_t storage_handle;
    esp_err_t err; 
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &storage_handle);
    do
    {
        if (err != ESP_OK)
        {
            Serial.println("Failed to open NVS");
            break;  
        }
        err = nvs_set_blob(storage_handle, JOIN_EUI_KEY, join_eui, 8);
        if (err != ESP_OK)
        {
            Serial.println("Failed to write join_eui");
            break;
        }
       
        err = nvs_commit(storage_handle);
        if (err != ESP_OK)
        {
            Serial.println("Failed to commit updated join_eui");
            break;
        }
        return_value = true;
    } while (0);
    nvs_close(storage_handle);
#else
    if(false == is_nvs_init)
    {
        return false;
    }

    if (0 == myMem.write(JOIN_EUI_LOC, join_eui, 8))
    {
        return_value = true;
    }
    else
    {
        return_value = false;
    }
#endif
    return return_value;
}   

bool nvs_storage_get_app_key(uint8_t *app_key)
{
    bool return_value = false;
#if USE_INTERNAL_FLASH
    nvs_handle_t storage_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &storage_handle);
    do
    {
        if (err != ESP_OK)
        {
            Serial.println("Failed to open NVS");
            break;
        }
        size_t required_size = 16;
        err = nvs_get_blob(storage_handle,APP_KEY_KEY, app_key, &required_size);
        
        if (err != ESP_OK)
        {
            Serial.println("Failed to read app_key");
            break;
        }
        return_value = true;
    } while (0);

    nvs_close(storage_handle);
#else
    if(false == is_nvs_init)
    {
        return false;
    }
    if (0 == myMem.read(APP_KEY_LOC, app_key, 16))
    {   
        if(is_all_ff(app_key, 16))
        {
            return_value = false;
        }
        else
        {
            return_value = true;
        }
    }
    else
    {
        return_value = false;
    }
#endif
    return return_value;
}

bool nvs_storage_set_app_key(uint8_t *app_key)
{
    bool return_value = false;
#if USE_INTERNAL_FLASH
    nvs_handle_t storage_handle;
    esp_err_t err;  
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &storage_handle);  
    do
    {           
        if (err != ESP_OK)
        {   
            Serial.println("Failed to open NVS");
            break;                  
        }
        err = nvs_set_blob(storage_handle, APP_KEY_KEY, app_key, 16); 
        if (err != ESP_OK)
        {   
            Serial.println("Failed to write app_key");
            break;                  
        }
        
        err = nvs_commit(storage_handle);
        if (err != ESP_OK)
        {   
            Serial.println("Failed to commit updated app_key");
            break;                  
        }
        return_value = true;    
    } while (0);
    nvs_close(storage_handle);
#else
    if(false == is_nvs_init)
    {
        return false;
    }

    if (0 == myMem.write(APP_KEY_LOC, app_key, 16))
    {
        return_value = true;
    }
    else
    {
        return_value = false;
    }
#endif
    return return_value;
}

bool nvs_storage_erase()
{
#if USE_INTERNAL_FLASH
    esp_err_t err = nvs_flash_erase();
    if (err == ESP_OK)
    {
        return true;
    }
    return false;
#else
    myMem.erase(0xFF);
    return true;
#endif
}

bool nvs_storage_get_lorawan_cred(uint8_t *app_key, uint8_t *join_eui, uint8_t *dev_eui)
{
    bool return_value = false;
    do
    {
        if (false ==nvs_storage_get_app_key(app_key))
        {
            Serial.println("Failed to read app_key from nvs_storage");
            break;
        }
        if (false ==nvs_storage_get_join_eui(join_eui))
        {
            Serial.println("Failed to read join_eui from nvs_storage");
            break;
        }
        if (false ==nvs_storage_get_dev_eui(dev_eui))
        {
            Serial.println("Failed to read dev_eui from nvs_storage");
            break;
        }
        return_value = true;
    } while (0);
    
    return return_value;

}



/******************************************************************************
 * END OF FILE
 ******************************************************************************/

