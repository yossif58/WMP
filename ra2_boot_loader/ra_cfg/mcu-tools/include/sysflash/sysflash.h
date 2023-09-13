/* generated configuration header file - do not edit */
#ifndef SYSFLASH_H_
#define SYSFLASH_H_
#ifndef __SYSFLASH_H__
#define __SYSFLASH_H__

#include "mcuboot_config/mcuboot_config.h"
#include "bsp_api.h"

#define FLASH_DEVICE_INTERNAL_FLASH    (0x7F)

#define FLASH_AREA_BOOTLOADER          0
#define FLASH_AREA_IMAGE_0             1
#define FLASH_AREA_IMAGE_1             2
#define FLASH_AREA_IMAGE_SCRATCH       3
#define FLASH_AREA_IMAGE_2             5
#define FLASH_AREA_IMAGE_3             6

#if (MCUBOOT_IMAGE_NUMBER == 1)
 #define FLASH_AREA_IMAGE_PRIMARY(x)      (((x) == 0) ?         \
                                           FLASH_AREA_IMAGE_0 : \
                                           FLASH_AREA_IMAGE_0)
 #define FLASH_AREA_IMAGE_SECONDARY(x)    (((x) == 0) ?         \
                                           FLASH_AREA_IMAGE_1 : \
                                           FLASH_AREA_IMAGE_1)

#elif (MCUBOOT_IMAGE_NUMBER == 2)

 #define FLASH_AREA_IMAGE_PRIMARY(x)      (((x) == 0) ?         \
                                           FLASH_AREA_IMAGE_0 : \
                                           ((x) == 1) ?         \
                                           FLASH_AREA_IMAGE_1 : \
                                           255)
 #define FLASH_AREA_IMAGE_SECONDARY(x)    (((x) == 0) ?         \
                                           FLASH_AREA_IMAGE_2 : \
                                           ((x) == 1) ?         \
                                           FLASH_AREA_IMAGE_3 : \
                                           255)

#else
#warning "Image slot and flash area mapping is not defined"
#endif

#define FLASH_AREA_MCUBOOT_OFFSET     (0x0)

#if (MCUBOOT_IMAGE_NUMBER == 1)

/* Secure + Non-Secure image primary slot */
 #ifndef FLASH_AREA_0_ID
  #define FLASH_AREA_0_ID               (FLASH_AREA_IMAGE_0)
 #endif
 #ifndef FLASH_AREA_0_OFFSET
  #define FLASH_AREA_0_OFFSET           (FLASH_AREA_MCUBOOT_OFFSET + RM_MCUBOOT_PORT_CFG_MCUBOOT_SIZE)
 #endif
 #ifndef FLASH_AREA_0_SIZE
  #define FLASH_AREA_0_SIZE             (RM_MCUBOOT_PORT_CFG_PARTITION_SIZE)
 #endif

/* Secure + Non-secure secondary slot */
 #ifndef FLASH_AREA_2_ID
  #define FLASH_AREA_2_ID               (FLASH_AREA_IMAGE_1)
 #endif
 #ifndef FLASH_AREA_2_OFFSET
  #define FLASH_AREA_2_OFFSET           (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
 #endif
 #ifndef FLASH_AREA_2_SIZE
  #define FLASH_AREA_2_SIZE             (RM_MCUBOOT_PORT_CFG_PARTITION_SIZE)
 #endif

/* Swap space.  */
 #ifndef FLASH_AREA_SCRATCH_ID
  #define FLASH_AREA_SCRATCH_ID         (FLASH_AREA_IMAGE_SCRATCH)
 #endif
 #ifndef FLASH_AREA_SCRATCH_OFFSET
  #define FLASH_AREA_SCRATCH_OFFSET     (FLASH_AREA_2_OFFSET + FLASH_AREA_2_SIZE)
 #endif
 #ifndef FLASH_AREA_SCRATCH_SIZE
  #define FLASH_AREA_SCRATCH_SIZE       (RM_MCUBOOT_PORT_CFG_SCRATCH_SIZE)
 #endif

#elif (MCUBOOT_IMAGE_NUMBER == 2)

/* Swap space.  */
 #ifndef FLASH_AREA_SCRATCH_ID
  #define FLASH_AREA_SCRATCH_ID         (FLASH_AREA_IMAGE_SCRATCH)
 #endif
 #ifndef FLASH_AREA_SCRATCH_OFFSET
  #define FLASH_AREA_SCRATCH_OFFSET     (FLASH_AREA_MCUBOOT_OFFSET + RM_MCUBOOT_PORT_CFG_MCUBOOT_SIZE)
 #endif
 #ifndef FLASH_AREA_SCRATCH_SIZE
  #define FLASH_AREA_SCRATCH_SIZE       (RM_MCUBOOT_PORT_CFG_SCRATCH_SIZE)
 #endif

/* Secure image secondary slot */
 #ifndef FLASH_AREA_2_ID
  #define FLASH_AREA_2_ID               (FLASH_AREA_IMAGE_2)
 #endif
 #ifndef FLASH_AREA_2_OFFSET
  #define FLASH_AREA_2_OFFSET           (FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE)
 #endif
 #ifndef FLASH_AREA_2_SIZE
  #define FLASH_AREA_2_SIZE             (RM_MCUBOOT_PORT_CFG_S_PARTITION_SIZE)
 #endif

/* Secure image primary slot */
 #ifndef FLASH_AREA_0_ID
  #define FLASH_AREA_0_ID               (FLASH_AREA_IMAGE_0)
 #endif
 #ifndef FLASH_AREA_0_OFFSET
  #define FLASH_AREA_0_OFFSET           (FLASH_AREA_2_OFFSET + FLASH_AREA_2_SIZE)
 #endif
 #ifndef FLASH_AREA_0_SIZE
  #define FLASH_AREA_0_SIZE             (RM_MCUBOOT_PORT_CFG_S_PARTITION_SIZE)
 #endif

/* Non-Secure image primary slot */
 #ifndef FLASH_AREA_1_ID
  #define FLASH_AREA_1_ID               (FLASH_AREA_IMAGE_1)
 #endif
 #ifndef FLASH_AREA_1_OFFSET
  #define FLASH_AREA_1_OFFSET           (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
 #endif
 #ifndef FLASH_AREA_1_SIZE
  #define FLASH_AREA_1_SIZE             (RM_MCUBOOT_PORT_CFG_NS_PARTITION_SIZE)
 #endif

/* Non-Secure image secondary slot */
 #ifndef FLASH_AREA_3_ID
  #define FLASH_AREA_3_ID               (FLASH_AREA_IMAGE_3)
 #endif
 #ifndef FLASH_AREA_3_OFFSET
  #define FLASH_AREA_3_OFFSET           (FLASH_AREA_1_OFFSET + FLASH_AREA_1_SIZE)
 #endif
 #ifndef FLASH_AREA_3_SIZE
  #define FLASH_AREA_3_SIZE             (RM_MCUBOOT_PORT_CFG_NS_PARTITION_SIZE)
 #endif
#else                                  /* MCUBOOT_IMAGE_NUMBER > 2 */
#error "Only MCUBOOT_IMAGE_NUMBER 1 and 2 are supported!"
#endif /* MCUBOOT_IMAGE_NUMBER */

#endif /* __SYSFLASH_H__ */
#endif /* SYSFLASH_H_ */
