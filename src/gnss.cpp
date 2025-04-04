#include <Arduino.h>
// #include <Adafruit_NeoPixel.h>
// #include "Adafruit_SHT4x.h"
#include "hal/uart_types.h"
#include <TinyGPS++.h>
#include "gnss.h"

// Define GPS baud rate
static const uint32_t GPSBaud = 115200;

// Create TinyGPS++ object
TinyGPSPlus gps;

// Define hardware serial for GPS
#define RXPin 16 // GPS TX -> ESP32 RX (GPIO16)
#define TXPin 17 // GPS RX -> ESP32 TX (GPIO17)

#define GNSS_RESET_PIN 18 // Define GNSS reset pin

HardwareSerial GPS_Serial(2); // GNSS connected to Serial2

// ============================================================================
// Function to initialize GNSS module
// Returns true if initialization is successful, false otherwise
// ============================================================================
bool init_gnss(void)
{

    bool rtnVal = false;

    Serial.println("GNSS Start");

    // Set GNSS reset pin as output
    pinMode(GNSS_RESET_PIN, OUTPUT);

    // Reset GNSS
    digitalWrite(GNSS_RESET_PIN, LOW);
    delay(1000);
    digitalWrite(GNSS_RESET_PIN, HIGH);
    delay(1000);

    GPS_Serial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin); // Initialize Serial2 for GPS

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

    // Send command to query GPS baud rate
    Serial.println(F("Sending GNSS baud rate query command..."));
    static const char pair_gnss_baudrate[] = "$PAIR865,0,0*31\r\n";
    GPS_Serial.print(pair_gnss_baudrate);

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
        // Serial.print(c);
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

    return rtnVal; // Return the status of GPS data availability
}
