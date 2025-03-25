

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include "host_fuota.h"
#include "api_processor.h"
#include <Arduino.h>

/******************************************************************************
 * EXTERN VARIABLES
 ******************************************************************************/

/******************************************************************************
 * PRIVATE MACROS AND DEFINES
 ******************************************************************************/

/******************************************************************************
 * PRIVATE TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 * STATIC VARIABLES
 ******************************************************************************/

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


/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
uint32_t get_seg_size_bytes(uint8_t seg_size_type)
{
    uint32_t seg_size_bytes = 0;

    switch (seg_size_type)
    {
        case SEG_SIZE_64:
            seg_size_bytes = FUOTA_SEG_SIZE_BYTES_64;
            break;
        case SEG_SIZE_128:
            seg_size_bytes = FUOTA_SEG_SIZE_BYTES_128;
            break;
        case SEG_SIZE_256:
            seg_size_bytes = FUOTA_SEG_SIZE_BYTES_256;
            break;
        case SEG_SIZE_512:
            seg_size_bytes = FUOTA_SEG_SIZE_BYTES_512;
            break;
        default:
            break;
    }

    return seg_size_bytes;
}

uint32_t calculate_no_of_segments(uint8_t *pkg_size, uint32_t seg_size_byte)
{
   uint32_t pkg_full_size =  pkg_size[0] << 16 | pkg_size[1] << 8 | pkg_size[2];
    return (pkg_full_size + seg_size_byte - 1) / seg_size_byte;
}

bool is_all_segments_downloaded(get_seg_file_status_t seg_file_status)
{
    bool is_downloaded = true;
    uint32_t pkg_full_size =  seg_file_status.pkg_size[0] << 16 | seg_file_status.pkg_size[1] << 8 | seg_file_status.pkg_size[2];   
    // Determine the segment size based on the segment size type
    uint32_t seg_size = get_seg_size_bytes(seg_file_status.seg_size);
    if(seg_size == 0)
    {
        return false;
    }
    uint32_t total_segments = (pkg_full_size + seg_size - 1) / seg_size;
    Serial.printf("Total segments: %d\n", total_segments);
    for (uint32_t i = 0; i < total_segments; i++)
    {
        if ((seg_file_status.seg_status & (1 << i)) != 0)
        {
            is_downloaded = false;
            Serial.printf("Segment %d is not downloaded\n", i);
            break;
        }
    }

    return is_downloaded;
}
uint8_t get_cmd_type_from_seg_file_status(get_seg_file_status_t seg_file_status)
{
    return seg_file_status.cmd_type.cmd_type;
}
