/**
 * @file gnss.h
 * @author Rick Seltzer
 * @brief Header file for the gnss interface
 * @version 0.1
 * @date 2025-4-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __GNSS_H__
#define __GNSS_H__

#ifdef __cplusplus
extern "C"
{
#endif

    /**********************************************************************************************************
     * INCLUDES
     **********************************************************************************************************/

#include <TinyGPS++.h>


    /**********************************************************************************************************
     * STRUCTURES
     **********************************************************************************************************/
typedef struct
    {
        double latitude;  // Latitude in degrees
        double longitude; // Longitude in degrees
        double altitude;  // Altitude in meters
        double speed;     // Speed in meters per second
        float course;    // Course in degrees
        uint32_t numSat;   // Time in milliseconds since boot
    } gnss_data_t;

    /**********************************************************************************************************
     * PROTOTYPES
     **********************************************************************************************************/

    // Function to initialize GNSS module
    // Returns true if initialization is successful, false otherwise
    bool init_gnss(void);

    // Function to read serial port, encode data, and return with indication
    // that a new location result is ready
    bool gnssCheckin(gnss_data_t *gnss_data_rtn);

#ifdef __cplusplus
}
#endif

#endif // __GNSS_H__