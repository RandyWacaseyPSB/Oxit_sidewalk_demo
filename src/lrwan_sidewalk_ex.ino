/**
 * @file mcm_lorawan.ino
 * @author Ankit Bansal (ankit.bansal@oxit.com)
 * @brief This Sketch demonstrates the  connectivity of the mcm module.
 *        The module is  set to to connect to the lorawan network
 *        and then send uplink to the module.
 *        A temperature and humidity sensor is attached to the module
 *        Send the uplink every 30 seconds( This is configurable)
 *        Check the status of the last sent uplink
 * @version 0.1
 * @date 2024-07-08
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
#include "mcm_rover.h"
#include "oxit_cli_app.h"
#include "oxit_nvs.h"
#include <Adafruit_NeoPixel.h>
#include "Adafruit_SHT4x.h"
#include "hal/uart_types.h"
#include "SPIFFS.h"
#include "lrwan_sidewalk_ex.h"
#include "led_control.h"

/******************************************************************************
 * EXTERN VARIABLES
 ******************************************************************************/

/******************************************************************************
 * STATIC VARIABLES
 ******************************************************************************/

MCM mcm(Serial1, TX_PIN, RX_PIN, RESET_PIN);

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

/**
 * ╔═════════════════════════════════════════════════════════════════════════════╗
 * ║                         Lorawan Credentials                                 ║
 * ╚═════════════════════════════════════════════════════════════════════════════╝
 */
uint8_t dev_eui[]     = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t join_eui[]    = {0xFF, 0xFF, 0xFF, 0xFf, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t network_key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint8_t saved_dev_eui[8]      = {0};
uint8_t saved_join_eui[8]     = {0};
uint8_t saved_network_key[16] = {0};

static system_state currentState = STATE_NO_LORAWAN_CRED;

float temp = 0.0;
float hum  = 0.0;

uplink_data_t uplink_data;

static bool is_device_joined = false;

/**
 * @brief Device connection mode.
 *
 * Indicates the current connection mode used by the device.
 * The default value is ConnectionMode::CONNECTION_MODE_NC, which means no connection mode selected.
 */
ConnectionMode device_mode = ConnectionMode::CONNECTION_MODE_NC;

uint8_t is_device_have_valid_lorawan_credentials = 0;

/******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

// Add these global variables
volatile bool buttonPressed                = false;
volatile unsigned long lastButtonInterrupt = 0;
const unsigned long debounceTime           = 50; // 50ms debounce time

/******************************************************************************
 * STATIC FUNCTION PROTOTYPES
 ******************************************************************************/

/**
 * @brief Sets the connection mode of the MCM module.
 *
 * @param mode The connection mode to be set.
 */
static void set_mcm_connect_mode(ConnectionMode mode);

/**
 * @brief Joins the LoRaWAN network.
 *
 * @return True if successful, false otherwise.
 */
static bool initiate_network_join();

/**
 * @brief Reads the sensor values.
 *
 * @param[out] temperature A pointer to store the temperature value.
 * @param[out] humidity A pointer to store the humidity value.
 */
static void read_sensor(float *temperature, float *humidity);

/**
 * @brief Sends the uplink data.
 *
 * @param temperature The temperature value.
 * @param humidity The humidity value.
 *
 * @return True if successful, false otherwise.
 */
static bool send_uplink(float temperature, float humidity);

/**
 * @brief Handles the downlink data.
 */
static void handle_downlink();

/**
 * @brief Validates the LoRaWAN credentials.
 *
 * @param dev_eui The device EUI.
 * @param dev_eui_len The length of the device EUI.
 * @param join_eui The join EUI.
 * @param join_eui_len The length of the join EUI.
 * @param network_key The network key.
 * @param network_key_len The length of the network key.
 *
 * @return True if the credentials are valid, false otherwise.
 */
static bool validate_lorawan_creds(uint8_t *dev_eui, uint8_t dev_eui_len, uint8_t *join_eui, uint8_t join_eui_len, uint8_t *network_key, uint8_t network_key_len);

/**
 * @brief Checks if all the elements in the array are 0xFF.
 *
 * @param arr The array to be checked.
 * @param len The length of the array.
 *
 * @return True if all elements are 0xFF, false otherwise.
 */
bool is_all_ff(uint8_t *arr, uint8_t len);

void helper_print_hex_array(const uint8_t *arr, size_t len);

// This function handles the button press events, including debouncing and switching network modes.
static void handleButtonPress();


/**
 * @brief Checks the device connection status and updates the LED state accordingly.
 *
 */
static void check_device_connection();

/******************************************************************************
 * STATIC FUNCTIONS
 ******************************************************************************/

static void set_mcm_connect_mode(ConnectionMode mode)
{

    // set the connection mode lorawan or sidewalk
    mcm.set_connect_mode(mode);

    // for sidewalk we do not need the credentials
    if (ConnectionMode::CONNECTION_MODE_LORAWAN == mode)
    {
        // TODO:check the validity of lorawan credentials
        Serial.print("Device EUI: ");
        helper_print_hex_array(saved_dev_eui, 8);
        Serial.print("Join EUI: ");
        helper_print_hex_array(saved_join_eui, 8);
        Serial.print("Network key: ");
        helper_print_hex_array(saved_network_key, 16);
        // set credentials in case of lorawan
        mcm.set_lorawan_credentials(saved_dev_eui, saved_join_eui, saved_network_key);
    }
}

static bool initiate_network_join()
{
    Serial.println("Joining  network...\n");
    // connect to the network
    mcm.connect_network();
    return true;
}

static void read_sensor(float *temperature, float *humid)
{

    Serial.println("Reading sensor data...");
    sensors_event_t humidity, temp;
    if (sht4.getEvent(&humidity, &temp))

    {
        Serial.print("Temperature: ");
        Serial.print(temp.temperature);
        Serial.println(" degrees C");
        Serial.print("Humidity: ");
        Serial.print(humidity.relative_humidity);
        Serial.println("% rH");
    }
    else
    {
        Serial.println("Enable to read sensor data, using dummy values");
        temp.temperature           = 25.0;
        humidity.relative_humidity = 50.0;
    }

    *temperature = temp.temperature;
    *humid       = humidity.relative_humidity;
}

static bool send_uplink(float temperature, float humidity)
{
    bool uplink_done = false;

    do
    {
        if (mcm.is_connected() == false && (device_mode != ConnectionMode::CONNECTION_MODE_SIDEWALK_BLE))
        {
            Serial.println("Device not connected to network.\n");
            break;
        }

        set_led_state(LED_SENDING_UPLINK);
        // Code to send uplink with temperature and humidity data
        Serial.printf("Sending uplink: Temp = %.2f, Humidity = %.2f, Reboot counter = %d\r\n", temperature, humidity, uplink_data.reboot_count);

        uplink_data_t uplink_data;
        uplink_data.temp = (uint16_t)(temperature * 100);
        uplink_data.hum  = (uint16_t)(humidity * 100);
        // reboot counter is already assigned on bootup
        Serial.print("Uplink in hex: ");
        helper_print_hex_array((uint8_t *)&uplink_data, sizeof(uplink_data));

        mcm.send_uplink((uint8_t *)&uplink_data, sizeof(uplink_data), LORAWAN_PORT, MCM_UPLINK_TYPE::MCM_UPLINK_TYPE_CONF);

        uplink_done = true; // Assume success if we reach this point

    } while (0); // Loop only once

    // Return true if uplink is successful
    return uplink_done;
}

static void handle_downlink()
{
    // lets check if any download is available
    if (mcm.is_downlink_available())
    {
        set_led_state(LED_RECEIVED_DOWNLINK);

        Serial.println("--------------------Downlink available--------------------");
        // retrieve the downlink and meta data
        uint8_t received_data[BUFFER_SIZE];
        uint16_t received_len;
        int8_t rssi;
        int8_t snr;
        uint16_t seq_port;

        mcm.get_downlink_data(received_data, &received_len, &rssi, &snr, &seq_port);

        Serial.printf("Rssi: %d\r\n", rssi);
        Serial.printf("Snr: %d\r\n", snr);
        if (ConnectionMode::CONNECTION_MODE_LORAWAN == mcm.get_connect_mode())
        {
            // if mode is lorawan then its port
            Serial.printf("Lorawan port: %d\n", seq_port);
        }
        else
        {
            // if mode is sidewalk then its sidewalk sequence
            Serial.printf("Sidewalk sequence: %d\n", seq_port);
        }

        Serial.printf("Receivced payload Size: %d\n", received_len);
        Serial.println("Received Downlink data: ");
        helper_print_hex_array(received_data, received_len);
        Serial.printf("\n");

        Serial.println("----------------------------------------------------");
    }
}

bool is_all_ff(uint8_t *arr, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        if (arr[i] != 0xff)
        {
            return false;
        }
    }
    return true;
}

static bool validate_lorawan_creds(uint8_t *dev_eui, uint8_t dev_eui_len, uint8_t *join_eui, uint8_t join_eui_len, uint8_t *network_key, uint8_t network_key_len)
{
    bool return_value = false;
    do
    {
        if (dev_eui == NULL || join_eui == NULL || network_key == NULL)
        {
            Serial.println("Invalid credentials");
            break;
        }
        if (dev_eui_len != 8 || join_eui_len != 8 || network_key_len != 16)
        {
            Serial.println("Invalid credentials length");
            break;
        }

        if (true == is_all_ff(dev_eui, dev_eui_len))
        {
            Serial.println("Invalid dev_eui");
            break;
        }
        if (true == is_all_ff(join_eui, join_eui_len))
        {
            Serial.println("Invalid join_eui");
            break;
        }
        if (true == is_all_ff(network_key, network_key_len))
        {
            Serial.println("Invalid network_key");
            break;
        }

        return_value = true;

    } while (0);

    return return_value;
}

// ISR function
void IRAM_ATTR buttonISR()
{
    unsigned long currentTime = millis();
    if (currentTime - lastButtonInterrupt > debounceTime)
    {
        buttonPressed       = true;
        lastButtonInterrupt = currentTime;
    }
}

/**
 * @brief Switches to the specified network mode.
 *
 * This function stops the current network, validates credentials if necessary, sets the new mode, and updates the state.
 * @param new_mode The new connection mode to switch to.
 */
void switch_protocol_mode(ConnectionMode new_mode)
{
    // Check if switching to LoRaWAN and validate credentials
    if (new_mode == ConnectionMode::CONNECTION_MODE_LORAWAN && !is_device_have_valid_lorawan_credentials)
    {
        Serial.println("Invalid LoRaWAN credentials, not switching to LoRaWAN");
        return;
    }

    // Stop the current network connection
    mcm.stop_network();
    delay(500);

    set_led_state(LED_DEVICE_NOT_CONNECTED);
    is_device_joined = false;

    // Log the mode switching
    Serial.print("Switching to ");
    switch (new_mode)
    {
        case ConnectionMode::CONNECTION_MODE_LORAWAN:
            Serial.println("LoRaWAN");
            break;
        case ConnectionMode::CONNECTION_MODE_SIDEWALK_BLE:
            Serial.println("Sidewalk BLE");
            break;
        case ConnectionMode::CONNECTION_MODE_SIDEWALK_FSK:
            Serial.println("Sidewalk FSK");
            break;
        case ConnectionMode::CONNECTION_MODE_SIDEWALK_CSS:
            Serial.println("Sidewalk CSS");
            break;
        default:
            Serial.println("No Connection (NC)");
            break;
    }

    // Set the new connection mode
    device_mode = new_mode;

    // Update the current state to set the new connection mode
    currentState = STATE_SET_CONNECT_MODE;
}

void helper_print_hex_array(const uint8_t *arr, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        Serial.printf("0x%02X", arr[i]);
        if (i != len - 1)
        {
            Serial.printf(",", arr[i]);
        }
    }
    Serial.println();
}

static void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("An error has occurred while mounting SPIFFS");
        return;
    }
    Serial.println("SPIFFS mounted successfully");
}

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/

/******************************************************************************
 * Function Definitions
 *******************************************************************************/

void setup()
{
    delay(1000);
    Serial.begin(115200);
    Serial.println("Starting setup");

#if ENABLE_MANUFACTURING_MODE

    Serial.println("Enabling manufacturing mode");
    Serial.println("This mode helps in flashing the manufacturing file in mcm by not issuing any command to the mcm");
    Serial.println("Please disable the manufacturing mode manually after flashing the manufacturing file");
    Serial.println("To disable the manufacturing mode, set ENABLE_MANUFACTURING_MODE to 0 in the .ino file");

#else

    Serial.println("Starting setup");

    initSPIFFS();

    pinMode(MCM_EVK_USER_LED, OUTPUT);
    digitalWrite(MCM_EVK_USER_LED, HIGH);

    // initiating the neo pixel
    led_control_init(); // Initialize the LED control

    // Initiate the boot-up sequence for the NeoPixel
    led_boot_up();
    set_led_state(LED_DEVICE_NOT_CONNECTED); // Set LED state for not connected

    pinMode(I2C_POWER_PIN, OUTPUT);
    digitalWrite(I2C_POWER_PIN, HIGH);
    Wire.setPins(I2C_SDA_PIN, I2C_SCL_PIN);

    Serial.println("Adafruit SHT4x test");
    if (!sht4.begin())
    {
        Serial.println("Couldn't find SHT4x");
    }
    else
    {
        Serial.println("Found SHT4x sensor");
        Serial.print("SHT4x sensor Serial number 0x");
        Serial.println(sht4.readSerial(), HEX);
        sht4.setPrecision(SHT4X_HIGH_PRECISION);
        sht4.setHeater(SHT4X_NO_HEATER);
    }

    if (false == nvs_storage_init())
    {
        Serial.println("Failed to initialize NVS storage");
    }

    else
    {
        Serial.println("Successfully initialized NVS storage");
    }

    // Initialize the command line interface application
    // This sets up the command line interface with the application specific commands,.
    init_command_line_app();

    // Configure button pin with pull-up and attach ISR
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

    // Initialize peripherals and callback functions etc. related to serial interface with MCM.
    MCM_STATUS status = mcm.begin();

    if (MCM_STATUS::MCM_OK != status)
    {
        Serial.println("mcm begin failed");
    }
    // Enable/disable debug serial logs (logs will be available on the default serial port of the Arduino board being used)
    // Caution: Turning debug logs on can flood the serial logs
    mcm.set_debug_enabled(true);

    // reboot the module
    mcm.hw_reset();

    ver_type_1_t mcm_rover_lib_ver, c_lib_ver;
    mcm.retrieveLibraryVersions(&mcm_rover_lib_ver, &c_lib_ver);
    Serial.println("MCM Rover Library Version: " + String(mcm_rover_lib_ver.major) + "." + String(mcm_rover_lib_ver.minor) + "." + String(mcm_rover_lib_ver.patch));
    Serial.println("C Library Version: " + String(c_lib_ver.major) + "." + String(c_lib_ver.minor) + "." + String(c_lib_ver.patch));

    // set application version here
    ver_type_1_t host_ver;
    host_ver.major = HOST_APP_VERSION_MAJOR;
    host_ver.minor = HOST_APP_VERSION_MINOR;
    host_ver.patch = HOST_APP_VERSION_PATCH;
    mcm.set_host_app_version(host_ver);
    Serial.println("Host App version: " + String(host_ver.major) + "." + String(host_ver.minor) + "." + String(host_ver.patch));

    // get reset count from nvs flash
    uplink_data.reboot_count = nvs_storage_get_reboot_count();
    Serial.println("Reboot count: " + String(uplink_data.reboot_count));

    // Start in sidewalk BLE mode regardless of the validity of LoRaWAN credentials
    Serial.println("Starting in sidewalk BLE mode");
    currentState                             = STATE_SET_CONNECT_MODE;                       // Set to sidewalk mode
    is_device_have_valid_lorawan_credentials = 0;                                            // No valid credentials initially
    device_mode                              = ConnectionMode::CONNECTION_MODE_SIDEWALK_FSK; // Set device mode to FSK

    // Check if credentials are available in the NVS storage
    if (true == nvs_storage_get_lorawan_cred(saved_network_key, saved_join_eui, saved_dev_eui))
    {
        Serial.println("Loaded saved lorawan credentials from nvs storage");
        is_device_have_valid_lorawan_credentials = true;
        // No need to switch to LoRaWAN mode
    }
    // If hardcoded credentials are valid
    else if (validate_lorawan_creds(dev_eui, sizeof(dev_eui), join_eui, sizeof(join_eui), network_key, sizeof(network_key)))
    {
        Serial.println("Loaded hardcoded credentials");
        memcpy(saved_dev_eui, dev_eui, sizeof(dev_eui));
        memcpy(saved_join_eui, join_eui, sizeof(join_eui));
        memcpy(saved_network_key, network_key, sizeof(network_key));
        // No need to switch to LoRaWAN mode
    }
    else
    {
        Serial.println("No valid credentials available, Please enter credentials manually for lorawan");
    }

    get_seg_file_status_t file_status;
    // send get segment command
    mcm.get_segmented_file_download_status(&file_status);

#endif
}

void loop()
{
#if ENABLE_MANUFACTURING_MODE
    // do nothing
#else
    process_blink_requests(); // Add this line to process LED states
    // Run the state machine to handle the current application state
    // This function checks the current state and performs the appropriate actions
    // based on the current state. This function is called repeatedly in the loop function.
    run_state_machine();

    // process the events received from MCM
    mcm.handle_rx_events();

    // handling the cli data from the command line
    process_command_line_app();

    // Call the handleButtonPress function to handle button press and debounce
    handleButtonPress();

#endif
}

/**
 * @brief Array of state names corresponding to the system_state enum.
 */
const char *state_names[] = {"STATE_SET_CONNECT_MODE", "STATE_JOIN_NETWORK",    "STATE_READ_SENSOR",          "STATE_SEND_UPLINK", "STATE_UPLINK_STATUS", "STATE_IDLE",
                             "STATE_NO_LORAWAN_CRED",  "STATE_FIRMWARE_UPDATE", "STATE_HOST_FIRMWARE_UPDATE", "UNKNOWN_STATE"};

/**
 * @brief Sets the current state of the system and prints the state name to the Serial monitor.
 *
 * @param state_value The new state to set.
 */
void set_state(system_state state_value)
{
    Serial.printf("Setting state to: %s\n", state_names[state_value]);
    currentState = state_value;
}

void run_state_machine()
{
    static uint32_t last_uplink_time = millis();

    // check for new binary file downloaded
    if (mcm.is_new_firmware())
    {
        // Check binary type from segment file status
        if (mcm.seg_file_status.cmd_type.bin_type == FUOTA_BINARY_TYPE_MCM)
        {
            Serial.println("New MCM firmware downloaded");
        }
        else if (mcm.seg_file_status.cmd_type.bin_type == FUOTA_BINARY_TYPE_HOST)
        {
            Serial.println("New Host firmware downloaded");
        }
        else
        {
            Serial.println("Unknown binary type downloaded");
        }
        set_state(STATE_FIRMWARE_UPDATE);
    }

    check_device_connection();

    // State machine logic
    switch (currentState)
    {
        case STATE_SET_CONNECT_MODE: {
            set_mcm_connect_mode(device_mode);
            set_state(STATE_JOIN_NETWORK);
            break;
        }

        case STATE_JOIN_NETWORK: {
            if (initiate_network_join())
            {
                set_state(STATE_READ_SENSOR);
            }
            break;
        }

        case STATE_READ_SENSOR: {
            read_sensor(&temp, &hum);
            set_state(STATE_SEND_UPLINK);
            break;
        }

        case STATE_SEND_UPLINK: {
            // if uplink is done then go to the next state
            // otherwise keep in idle state
            if (send_uplink(temp, hum))
            {
                set_state(STATE_UPLINK_STATUS);
                last_uplink_time = millis();
            }
            else
            {
                set_state(STATE_IDLE);
            }
            break;
        }

        case STATE_UPLINK_STATUS: {
            // check for the uplink status
            // Check if the uplink is pending or transmitted
            if (mcm.is_last_uplink_pending() && millis() - last_uplink_time < UPLINK_NO_RESPONSE_TIMEOUT_SECONDS * 1000)
            {
                if (currentState != STATE_UPLINK_STATUS)
                {
                    // keep checking the uplink status
                    set_state(STATE_UPLINK_STATUS);
                }
            }
            else
            {
                switch (mcm.get_last_tx_status())
                {
                    case MCM_TX_STATUS::MCM_TX_NOT_SEND:

                        Serial.println("last uplink failed");
                        set_led_state(LED_SENDING_UPLINK_FAIL);
                        break;
                    case MCM_TX_STATUS::MCM_TX_WO_ACK:
                        Serial.println("last uplink sent successfully without ack");
                        break;

                    case MCM_TX_STATUS::MCM_TX_ACK:
                        Serial.println("last uplink sent successfully with ack");
                        break;
                    default:
                        if (false == mcm.is_connected())
                        {
                            Serial.println("MCM is not joined to any network");
                            break;
                        }
                        Serial.println("Uplink failed reason unknown!");
                        break;
                }
                Serial.println();
                set_state(STATE_IDLE);
            }
        }
        break;

        case STATE_NO_LORAWAN_CRED:
            break;

        case STATE_IDLE: {
            // do nothing if ymodem transfer is going on
            if (mcm.ymodem.getState() != YMODEM_IDLE)
            {
                break;
            }
            // Handle downlink if any
            handle_downlink();

            // If MCM has been rebooted, set the connection mode again
            if (mcm.get_context_mgr_is_mcm_reset())
            {
                get_seg_file_status_t file_status;
                // send get segment command
                mcm.get_segmented_file_download_status(&file_status);
                // Serial.println("MCM reset detected, connecting again");
                is_device_joined = false;
                set_led_state(LED_DEVICE_NOT_CONNECTED); // Set LED state for not connected
                set_state(STATE_SET_CONNECT_MODE);
            }

            // Uplink every N seconds
            if (millis() - last_uplink_time > UPLINK_INTERVAL_SECONDS * 1000)
            {
                last_uplink_time = millis();
                set_state(STATE_READ_SENSOR);
            }
            break;
        }
        case STATE_FIRMWARE_UPDATE: {
            Serial.println("Do you want to proceed with firmware update? (y/n)");
            unsigned long start_time = millis();
            while (!Serial.available())
            {
                if (millis() - start_time > 30000)
                { // 30 seconds timeout
                    Serial.println("Timeout waiting for input.");
                    break; // Exit the function or handle timeout as needed
                }
            }

            char response = Serial.read();

            // Clear any remaining characters in buffer
            while (Serial.available())
            {
                Serial.read();
            }

            if (response == 'y' || response == 'Y')
            {
                Serial.println("Proceeding with firmware update...");
                mcm.process_fw_update();
                set_state(STATE_IDLE);
                delay(1000);
            }
            else
            {
                Serial.println("Firmware update cancelled");
                set_state(STATE_SET_CONNECT_MODE);
            }

            break;
        }

        default:
            // Handle unexpected states
            break;
    }
}

void send_uplink_now()
{
    currentState = STATE_READ_SENSOR;
}

void send_fw_update_request()
{
    get_seg_file_status_t file_status;
    // send get segment command
    mcm.get_segmented_file_download_status(&file_status);
}

static void handleButtonPress()
{
    if (buttonPressed)
    {
        buttonPressed = false; // Clear the flag

        /* Debounce check
        if (millis() - lastButtonInterrupt < debounceTime)
        {
            return;
        }
*/

        Serial.println("Button pressed - Switching Network");

        // State machine to cycle through Sidewalk protocols only
        switch (device_mode)
        {
            case ConnectionMode::CONNECTION_MODE_SIDEWALK_BLE:
                device_mode = ConnectionMode::CONNECTION_MODE_SIDEWALK_FSK;
                break;
            case ConnectionMode::CONNECTION_MODE_SIDEWALK_FSK:
                device_mode = ConnectionMode::CONNECTION_MODE_SIDEWALK_CSS;
                break;
            case ConnectionMode::CONNECTION_MODE_SIDEWALK_CSS:
                device_mode = ConnectionMode::CONNECTION_MODE_SIDEWALK_BLE; // Cycle back to BLE
                break;
            default:
                device_mode = ConnectionMode::CONNECTION_MODE_SIDEWALK_BLE; // Default to Sidewalk BLE if in an unknown state
                break;
        }

        switch_protocol_mode(device_mode);
    }
}

/**
 * @brief Checks the device connection status and updates the LED state accordingly.
 *
 * This function checks if the device is connected to the network. If the device
 * is connected and has not previously joined, it updates the joined state and
 * sets the appropriate LED state based on the current connection mode. If the
 * device is not connected, it resets the joined state and indicates the disconnection
 * through the LED.
 */
static void check_device_connection()
{
    if (true == mcm.is_connected())
    {
        if (false == is_device_joined)
        {
            is_device_joined = true;
            Serial.println("Device joined successfully");

            switch (device_mode)
            {
                case ConnectionMode::CONNECTION_MODE_LORAWAN:
                    set_led_state(LED_JOINED_LORAWAN_NETWORK);
                    break;
                case ConnectionMode::CONNECTION_MODE_SIDEWALK_BLE:
                    set_led_state(LED_JOINED_SW_BLE_NETWORK);
                    break;
                case ConnectionMode::CONNECTION_MODE_SIDEWALK_FSK:
                    set_led_state(LED_JOINED_SW_FSK_NETWORK);
                    break;
                case ConnectionMode::CONNECTION_MODE_SIDEWALK_CSS:
                    set_led_state(LED_JOINED_SW_CSS_NETWORK);
                    break;
                default:
                    Serial.println("No Connection (NC)");
                    break;
            }
        }
    }
    else
    {
        if (is_device_joined && device_mode != ConnectionMode::CONNECTION_MODE_SIDEWALK_BLE)
        {
            Serial.println("Device not connected to network.");
            set_led_state(LED_DEVICE_NOT_CONNECTED); // Set LED state for not connected
            is_device_joined = false;                // Reset the joined state if not connected
        }
    }
}
