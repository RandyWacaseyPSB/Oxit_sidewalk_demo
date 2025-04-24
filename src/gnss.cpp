#include <Arduino.h>
#include "hal/uart_types.h"
#include <TinyGPS++.h>
#include "gnss.h"

// #####################################################################
// Build Defs
// #####################################################################
// #define PRINT_ALL_QUECTEL_RESPONSES

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// GNSS CONFIG
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
#define QUECTEL_QUERY_FW_VER "$PQTMVERNO*58" // Query Quectel FW version

// PAIR COMMANDS
// ===================
#define QUECTEL_SET_FIX_RATE_1HZ_PAIR      "$PAIR050,1000*12" // Set Quectel fix rate to 1Hz (1000 = mS)
#define QUECTEL_SET_FIX_RATE_1_HZ_PAIR_RSP "PAIR001,050,0"    // Set Quectel fix rate response

//--
// Enable Quectel constellations: GPS, BDS, Galileo, GLONASS
//  <GPS>,<GLONASS>,<Galileo>,<BDS>,<QZSS>,<NavIC>
#define QUECTEL_ENABLE_CONSTELLATIONS_PAIR "$PAIR066,1,1,1,1,0,0*3A"
#define QUECTEL_ENAB_CONSTELL_PAIR_RSP     "PAIR001,066,0"

// ONLY SENTENCES USED IN TinyGPS++ Lib = GGA and RMC
//--
#define QUECTEL_GGA_EVERY_POS_FIX_PAIR "$PAIR062,0,1*3F" // Set GGA message to output every position fix
#define QUECTEL_RMC_EVERY_POS_FIX_PAIR "$PAIR062,4,1*3B" // Set RMC message to output every position fix
#define QUECTEL_POS_FIX_PAIR_RSP       "PAIR001,062,0"   // Response to all

// Output rates (per fix; 1 = every fix)
#define QUECTEL_GGA_EVERY_POS_FIX     "$PQTMCFGMSGRATE,W,GGA,1*0A" // Set GGA message to output every position fix
#define QUECTEL_RMC_EVERY_POS_FIX     "$PQTMCFGMSGRATE,W,RMC,1*17" // Set RMC message to output every position fix
#define QUECTEL_RMC_EVERY_POS_FIX_RSP "OK"                         // Set RMC message rate response

//--
#define QUECTEL_WARM_START_PAIR     "$PAIR005*3F"    // Warm start Quectel
#define QUECTEL_WARM_START_PAIR_RSP "$PAIR001,005,0" // Warm start Quectel

//---------------------

// Buffer sizes
// ------------------------
// This is required due to need to retrieve messages without auto-callbacks
#define MAX_EOE_BUF   (2048)
#define MAX_RESP_SIZE (256)

// Default time to wait for a response from the GNSS module
#define TYP_CMD_RESP_TIMEOUT_ms (5000)

// ******************************************************************************
//  * PRIVATE FUNCTION PROTOTYPES
//  ******************************************************************************
// bool sendGPSCommand(const char *command, const char *expectedResponse, unsigned long timeout = TYP_CMD_RESP_TIMEOUT_ms);

// Low level "send" to GNSS
void sendCommandToGNSS(const char *command);

// Low level "send" to GNSS with length
void sendDataToGNSS(char *cmd, int len);

// low level "receive" from GNSS for a single command response
bool waitForGNSSResponse(unsigned long timeout_ms, char *response_buffer, size_t max_size);

// bool sendGPSCommand(const char *command, const char *expectedResponse, unsigned long timeout);
bool sendGPSCommand(const char *command, const char *expectedResponse, unsigned long timeout);

// ============================================================================

HardwareSerial GPS_Serial(2); // GNSS connected to Serial2

// Define GPS baud rate
static const uint32_t GPSBaud = 115200;

// Create TinyGPS++ object
TinyGPSPlus gps;

// Define hardware serial for GPS
#define RXPin 16 // GPS TX -> ESP32 RX (GPIO16)
#define TXPin 17 // GPS RX -> ESP32 TX (GPIO17)

#define GNSS_RESET_PIN 18 // Define GNSS reset pin

// ============================================================================
// Function to initialize GNSS module
// Returns true if initialization is successful, false otherwise
// ============================================================================
bool init_gnss(void)
{

    bool rtnVal = false;
    bool cmdRsp;
    char rspBuf[MAX_RESP_SIZE] = {0};

    Serial.println("GNSS Start");

    // Set GNSS reset pin as output
    pinMode(GNSS_RESET_PIN, OUTPUT);

    // Reset GNSS
    digitalWrite(GNSS_RESET_PIN, LOW);
    delay(1000);
    digitalWrite(GNSS_RESET_PIN, HIGH);
    delay(2000);

    GPS_Serial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin); // Initialize Serial2 for GPS
    GPS_Serial.setRxBufferSize(2048);                    // Set receive buffer to 2048 bytes
                                                         // due to Quectel debug noise

    // Time for port to start
    delay(1000);

    // Check if the GPS module is connected
    if (GPS_Serial.available())
    {
        Serial.println(F("GNSS Module Connected"));
        rtnVal = true;
    }
    else
    {
        Serial.println(F("GNSS Module Not Found!"));
        rtnVal = false;
    }

    Serial.println(F("GNSS Module Connected via Serial2"));

    delay(2000);

    // set constellations - REBOOTS GNSS !!!!!!!!!!!!!!!!!!!!!
    // ---------------------------------------------------------
    const char *setConstellationsCommand = QUECTEL_ENABLE_CONSTELLATIONS_PAIR;
    const char *setConstellationsRsp     = QUECTEL_ENAB_CONSTELL_PAIR_RSP;
    cmdRsp                               = sendGPSCommand(setConstellationsCommand, setConstellationsRsp, TYP_CMD_RESP_TIMEOUT_ms);
    if (cmdRsp)
    {
        Serial.println("GPS constellations set: GPS, GLONASS, Galileo, BDS");
    }
    else
    {
        Serial.println("Failed to set constellations");
    }

    // Add a small delay between commands
    delay(5000);

    // =========================================================================

    // set update rate to 1 Hz
    // ---------------------------------------------------------
    const char *updateRateCommand = QUECTEL_SET_FIX_RATE_1HZ_PAIR;
    const char *updateRateRsp     = QUECTEL_SET_FIX_RATE_1_HZ_PAIR_RSP;
    cmdRsp                        = sendGPSCommand(updateRateCommand, updateRateRsp, TYP_CMD_RESP_TIMEOUT_ms);
    if (cmdRsp)
    {
        Serial.println("GPS update rate configured.");
    }
    else
    {
        Serial.println("Failed to configure GPS update rate");
    }

    // Add a small delay between commands
    delay(100);

    // RMC Every fix
    // ---------------------------------------------------------
    const char *enableRmcMsgCommand = QUECTEL_RMC_EVERY_POS_FIX_PAIR;
    const char *enableRmcMsgRsp     = QUECTEL_POS_FIX_PAIR_RSP;
    cmdRsp                          = sendGPSCommand(enableRmcMsgCommand, enableRmcMsgRsp, TYP_CMD_RESP_TIMEOUT_ms);
    if (cmdRsp)
    {
        Serial.println("Enable RMC Messages OK");
    }
    else
    {
        Serial.println("Enable RMC Message FAIL");
    }

    // Add a small delay between commands
    delay(100);

    // GGA Every fix
    // ---------------------------------------------------------
    const char *GGAMsgCommand = QUECTEL_GGA_EVERY_POS_FIX_PAIR;
    const char *GGAMsgRsp     = QUECTEL_POS_FIX_PAIR_RSP;
    cmdRsp                    = sendGPSCommand(GGAMsgCommand, GGAMsgRsp, TYP_CMD_RESP_TIMEOUT_ms);
    if (cmdRsp)
    {
        Serial.println("Enable GGA Messages OK");
    }
    else
    {
        Serial.println("Enable GGA Msg Fail");
    }

    // Add a small delay between commands
    delay(100);

    // =========================================================================

    Serial.printf("END GNSS SETUP !!!!!!!!!!!!!!!!!!!!!!!!!!!! \r\n\r\n\r\n");

    return rtnVal;
}

// ============================================================================
// Function to read serial port, encode data, and return with indication
// that a new location result is ready
// ============================================================================
bool gnssCheckin(gnss_data_t *gnss_data_rtn)
{

    bool rtnVal = false;

    while (GPS_Serial.available())
    {                               // If data is available from GPS
        char c = GPS_Serial.read(); // Read one character
        gps.encode(c);              // Process the character into TinyGPS++
#ifdef PRINT_ALL_QUECTEL_RESPONSES
        Serial.print(c);
#endif
    }

    // Check if new GPS location data is available
    if (gps.location.isUpdated())
    {

        rtnVal = true; // Set return value to true if location is updated
        // Return the data to the caller
        gnss_data_rtn->latitude  = gps.location.lat(); // Latitude in degrees
        gnss_data_rtn->longitude = gps.location.lng(); // Longitude in degrees

        // Set altitude if valid
        if (gps.altitude.isValid())
        {
            gnss_data_rtn->altitude = gps.altitude.meters(); // Altitude in meters
        }
        else
        {
            gnss_data_rtn->altitude = 0; // Set altitude to 0 if not valid
        }

        // Set speed if valid
        if (gps.speed.isValid())
        {
            gnss_data_rtn->speed = gps.speed.mps(); // Speed in meters per second
        }
        else
        {
            gnss_data_rtn->speed = 0; // Set speed to 0 if not valid
        }

        // Set course in degrees if valid
        if (gps.course.isValid())
        {
            gnss_data_rtn->course = gps.course.deg(); // Course in degrees
        }
        else
        {
            gnss_data_rtn->course = 0; // Set course to 0 if not valid
        }

        // if satellite data is valid, set number of satellites
        if (gps.satellites.isValid())
        {
            gnss_data_rtn->numSat = gps.satellites.value(); // Number of satellites
        }
        else
        {
            gnss_data_rtn->numSat = 0; // Set number of satellites to 0 if not valid
        }
    }

    // On a 5 second schedule: Print out the date and time from the GPS
    static uint32_t lastPrint = 0;
    if (millis() - lastPrint > 5000)
    {
        lastPrint = millis(); // Update the last print time

        // Print date and time if valid
        if (
            (gps.date.isValid())     && 
            (gps.time.isValid())     &&
            (gps.date.year() > 2024) &&   // was printing garbage dates until valid
            (gps.date.year() < 2035))
        {
            Serial.printf("Date: %02d/%02d/%02d, Time: %02d:%02d:%02d\r\n", gps.date.month(), gps.date.day(), gps.date.year(), gps.time.hour(), gps.time.minute(), gps.time.second());
        }
    }

    return rtnVal; // Return the status of GPS data availability
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// low level "send" data to GNSS
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void sendCommandToGNSS(const char *command)
{
    GPS_Serial.println(command);
    delay(50); // Brief delay to ensure command is processed
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// low level "send" data to GNSS
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void sendDataToGNSS(char *cmd, int len)
{
    // DO I NEED TO ADD CR/LF? $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    GPS_Serial.write((uint8_t *)cmd, len);
    delay(50); // Brief delay to ensure command is processed
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// GNSS "Send" command and wait for response
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define NUM_TIMES_RETRY_COMMAND (3)
bool sendGPSCommand(const char *command, const char *expectedResponse, unsigned long timeout /* = TYP_CMD_RESP_TIMEOUT_ms*/)
{

    static char response_buffer[MAX_RESP_SIZE]; // Buffer for response
    unsigned long startTime;
    uint16_t retry_counter = 0;

    while (retry_counter < NUM_TIMES_RETRY_COMMAND)
    {

        retry_counter++;

        startTime = millis(); // Record the start time

        sendCommandToGNSS(command); // Send the command
        Serial.printf("Sent command: %s\r\n", command);

        while (millis() - startTime < timeout) // Keep trying until timeout expires
        {
            memset(response_buffer, 0, sizeof(response_buffer)); // Clear the buffer

            bool gnssRespond = waitForGNSSResponse(timeout, response_buffer, (MAX_RESP_SIZE - 2));

            if (gnssRespond)
            {
                if (strstr(response_buffer, expectedResponse))
                {
                    Serial.printf("Received expected response: %s\r\n", response_buffer);
                    return true;
                }
                else
                {
                    // Serial.println("Received unexpected response: %s\r\n", response_buffer);
                }
            }
            else
            {
                Serial.printf("No valid response received. Retrying...\r\n");
            }
        }
    }
    Serial.printf("#######################    Timeout expired. Command failed:  %s  \r\n", command);
    return false; // Timeout expired without success
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Function to wait for a response from the GNSS module
// Parameters: timeout_ms - the maximum time to wait for a response
//             response_buffer - the buffer to store the response
//             max_size - the maximum size of the response buffer
// Returns: true if a full response was received, false if a timeout occurred
// or the buffer overflowed
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
bool waitForGNSSResponse(unsigned long timeout_ms, char *response_buffer, size_t max_size)
{
    unsigned long startTime = millis();
    size_t index            = 0;
    bool startDetected      = false;

    memset(response_buffer, 0, max_size); // Clear the buffer

    while (millis() - startTime < timeout_ms && index < max_size - 1)
    {
        while (GPS_Serial.available() && index < max_size - 1)
        {
            char c                   = GPS_Serial.read();
            response_buffer[index++] = c;

#ifdef PRINT_ALL_QUECTEL_RESPONSES
            Serial.printf("%c", c); // Echo raw GNSS data to Serial Monitor
#endif

            // Detect the start of a valid response (starting with '$')
            if (!startDetected && c == '$')
            {
                startDetected = true;
            }

            // Detect end of message (NMEA messages typically end with '\n')
            if (startDetected && c == '\n')
            {
                response_buffer[index] = '\0'; // Null-terminate the string
                return true;                   // Full response received
            }
        }
    }

    return false; // Response timeout or buffer overflow
}