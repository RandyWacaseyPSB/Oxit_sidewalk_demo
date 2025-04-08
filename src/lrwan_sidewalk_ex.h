#ifndef LRWAN_SIDEWALK_EX_H
#define LRWAN_SIDEWALK_EX_H

#include <stdint.h>
#include "mcm_rover.h"

/******************************************************************************
 * PRIVATE MACROS AND DEFINES
 ******************************************************************************/

// Pin configuration for the MCM module when host is ESP32S2/S3 Feather
#define TX_PIN    10
#define RX_PIN    9
#define RESET_PIN 14

// Uncomment the following lines for ESP32 host configuration
// #define TX_PIN 4
// #define RX_PIN 5

// LoRaWAN port number for sending uplink
#define LORAWAN_PORT 152

// Interval in seconds for sending sensor data as uplink
#define UPLINK_INTERVAL_SECONDS 15 /* 60 */

// Timeout in seconds for no response after last sent uplink
#define UPLINK_NO_RESPONSE_TIMEOUT_SECONDS 15 /* 60 */

// I2C interface configuration
#define I2C_POWER_PIN 7
#define I2C_SDA_PIN   3
#define I2C_SCL_PIN   4

// Push button configuration
#define BUTTON_PIN            0
#define BUTTON_DEBOUNCE_DELAY 1000

// MCM EVK User LED configuration
#define MCM_EVK_USER_LED 38

// RS485 interface configuration
#define PIN_RS485_EN 16
#define PIN_RS485_RX 18
#define PIN_RS485_TX 17
#define RS485        Serial2

// Manufacturing mode and version information
#define ENABLE_MANUFACTURING_MODE 0
#define HOST_APP_VERSION_MAJOR    0x00
#define HOST_APP_VERSION_MINOR    0x05
#define HOST_APP_VERSION_PATCH    0x00

// Payload - Data type (array[0])
#define DATA_TYPE_GNSS_BLE (1)
#define DATA_TYPE_GNSS_FSK (2)
#define DATA_TYPE_GNSS_CSS (3)
#define DATA_TYPE_GNSS_UNK (4)


#define MAX_USER_PAYLOAD (16) // Maximum user payload size for uplink data

/******************************************************************************
 * PRIVATE TYPEDEFS
 ******************************************************************************/

// Define system states
typedef enum
{
    STATE_SET_CONNECT_MODE,
    STATE_JOIN_NETWORK,
    STATE_READ_SENSOR,
    STATE_SEND_UPLINK,
    STATE_UPLINK_STATUS,
    STATE_IDLE,
    STATE_NO_LORAWAN_CRED,
    STATE_FIRMWARE_UPDATE,
} system_state;

// Structure to hold the uplink data
typedef struct
{
    uint8_t dta[MAX_USER_PAYLOAD]; /* NULL-terminated; max len = MAX_USER_PAYLOAD  */
    uint16_t reboot_count;
} uplink_data_t;

/**
 * @brief Switches to the specified network mode.
 *
 * This function stops the current network, validates credentials if necessary, sets the new mode, and updates the state.
 * @param new_mode The new connection mode to switch to.
 */
void switch_protocol_mode(ConnectionMode new_mode);

#endif // LRWAN_SIDEWALK_EX_H