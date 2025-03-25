#ifndef __HOST_FUOTA_DEFS_H__
#define __HOST_FUOTA_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
 * INCLUDES
 **********************************************************************************************************/
#include <stdint.h>
#include "api_processor.h"

/**********************************************************************************************************
 * MACROS AND DEFINES
 **********************************************************************************************************/
#define FUOTA_SEG_SIZE_BYTES_64              (64*1024)
#define FUOTA_SEG_SIZE_BYTES_128             (128*1024)
#define FUOTA_SEG_SIZE_BYTES_256             (256*1024)
#define FUOTA_SEG_SIZE_BYTES_512             (512*1024)

#define FUOTA_BINARY_TYPE_MCM          0x00
#define FUOTA_BINARY_TYPE_HOST         0x01

#define SEG_DOWNLOADED                  0x00
#define SEG_PENDING                     0x01

/**********************************************************************************************************
 * TYPEDEFS
 **********************************************************************************************************/
typedef enum
{
    SEG_SIZE_64 = 0x00,
    SEG_SIZE_128 = 0x01,
    SEG_SIZE_256 = 0x02,
    SEG_SIZE_512 = 0x03
} seg_size_t;


/**********************************************************************************************************
 * EXPORTED VARIABLES
 **********************************************************************************************************/

/**********************************************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 **********************************************************************************************************/
uint32_t get_seg_size_bytes(uint8_t seg_size_type);
uint32_t calculate_no_of_segments(uint8_t *pkg_size, uint32_t seg_size_byte);
bool is_all_segments_downloaded(get_seg_file_status_t seg_file_status);
uint8_t get_cmd_type_from_seg_file_status(get_seg_file_status_t seg_file_status);
#ifdef __cplusplus
}
#endif
#endif // __HOST_FUOTA_DEFS_H__

