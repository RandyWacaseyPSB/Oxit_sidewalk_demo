/**
 * @file ymodem.h
 * @author Ankit Bansal (ankit.bansal@oxit.com)
 * @brief
 * @version 0.1
 * @date 2024-12-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __YMODEM_H__
#define __YMODEM_H__

/**********************************************************************************************************
 * INCLUDES
 **********************************************************************************************************/
#include <stdint.h>
#include <Arduino.h>

/**********************************************************************************************************
 * MACROS AND DEFINES
 **********************************************************************************************************/

#define SOH 0x01   // Start of 128-byte data packet
#define STX 0x02   // Start of 1024-byte data packet
#define EOT 0x04   // End of transmission
#define ACK 0x06   // Acknowledge
#define NAK 0x15   // Negative Acknowledge
#define CRC16 0x43 // 'C' byte to request CRC16

#define YMODEM_TIMEOUT (30*1000)

/**********************************************************************************************************
 * TYPEDEFS
 **********************************************************************************************************/

typedef enum
{
    YMODEM_IDLE,
    WAIT_FOR_HEADER,
    RECEIVE_CHUNK,
    RECEIVE_DATA,
    WAIT_EOT,
    UPDATE_ESP32,
    COMPLETE
} ymodem_state_t;



class YModem
{
public:
    // Add constructor to initialize the reference
    YModem(HardwareSerial& serial) : __ymodem_serial(serial) {}

    void setState(ymodem_state_t state);

    void receivePacket(uint8_t *buffer, uint16_t &size);
    ymodem_state_t getState();
    void sendCRCRequest();
    bool update_esp32_firmware();
    void process_timeout();
    
private:
    ymodem_state_t _state = YMODEM_IDLE;
    HardwareSerial& __ymodem_serial;
    uint64_t _timeout;
    void sendACK();
    void sendNAK();

    // Calculates the CRC for a given data buffer
    uint16_t calculateCRC(const uint8_t *data, uint16_t length);
};

/**********************************************************************************************************
 * EXPORTED VARIABLES
 **********************************************************************************************************/

/**********************************************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 **********************************************************************************************************/

#endif // __YMODEM_H__