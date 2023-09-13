/* generated configuration header file - do not edit */
#ifndef MCUBOOT_CONFIG_H_
#define MCUBOOT_CONFIG_H_

/* Use the following commands to sign the image, replacing path/to/image.elf with the path to your image elf file.
 * Regenerate this template if any MCUboot properties change in the bootloader project.
 Make the following updates to convert to binary, then sign image 1 (primary slot: 1800, secondary/upgrade slot: 11000):
 1. Update path/to/image to reflect the location of the image 1 .elf file
 2. Update <boot_project> to reflect the location of the root of the bootloader project
 arm-none-eabi-objcopy -O binary path/to/image.elf path/to/image.bin
 python <boot_project>/ra/mcu-tools/MCUboot/scripts/imgtool.py sign --version 1.0.0+0 --header-size 0x100 --align 8 --max-align 8 --slot-size 0xf800 --max-sectors 31 --overwrite-only  --confirm --pad-header path/to/image.bin path/to/image_signed.bin
 */

#ifdef MCUBOOT_SIGN_RSA_LEN
#define MCUBOOT_SIGN_RSA
#endif

#define MCUBOOT_OVERWRITE_ONLY
#ifdef MCUBOOT_OVERWRITE_ONLY_FAST
#define MCUBOOT_OVERWRITE_ONLY
#endif
#ifdef MCUBOOT_DIRECT_XIP_REVERT
#define MCUBOOT_DIRECT_XIP
#endif

#if  (1 == 1)
#define MCUBOOT_USE_TINYCRYPT
#if  (1 == RA_NOT_DEFINED)
#define RM_MCUBOOT_PORT_USE_TINYCRYPT_ACCELERATION
#endif 
#elif (1 == RA_NOT_DEFINED)
#define MCUBOOT_USE_MBED_TLS
#else 
#define MCUBOOT_USE_USER_DEFINED_CRYPTO_STACK
#endif  

#ifdef MCUBOOT_USE_USER_DEFINED_CRYPTO_STACK
#include "sce9_ecdsa_p256.h"
#endif 

#define MCUBOOT_VALIDATE_PRIMARY_SLOT

#define MCUBOOT_DOWNGRADE_PREVENTION

#define MAX_BOOT_RECORD_SZ 0x64

#define MCUBOOT_SHARED_DATA_BASE 0x20000000

#define MCUBOOT_SHARED_DATA_SIZE 0x380

#define MCUBOOT_USE_FLASH_AREA_GET_SECTORS

#define MCUBOOT_IMAGE_NUMBER (1)

#if defined(MCUBOOT_LOG_LEVEL) && (MCUBOOT_LOG_LEVEL > 0)
#define MCUBOOT_HAVE_LOGGING    1
#endif

/*
 * Assertions
 */

/* Uncomment if your platform has its own mcuboot_config/mcuboot_assert.h.
 * If so, it must provide an ASSERT macro for use by bootutil. Otherwise,
 * "assert" is used. */
// #define MCUBOOT_HAVE_ASSERT_H
/*
 * Watchdog feeding
 */

#ifndef MCUBOOT_WATCHDOG_FEED
#define MCUBOOT_WATCHDOG_FEED()  
#endif

// do { /* Do nothing. */ } while (0)

#include "bsp_api.h"

#define RM_MCUBOOT_PORT_CFG_MCUBOOT_SIZE           (0x1800)
#if (MCUBOOT_IMAGE_NUMBER == 1)
#define RM_MCUBOOT_PORT_CFG_PARTITION_SIZE        (0xf800)
#else
 #define RM_MCUBOOT_PORT_CFG_S_PARTITION_SIZE      (0xf800)
#endif
#define RM_MCUBOOT_PORT_CFG_NS_PARTITION_SIZE      (0x0)
#define RM_MCUBOOT_PORT_CFG_SCRATCH_SIZE           (0x0)

#if BSP_FEATURE_FLASH_HP_VERSION > 0
 #define FLASH_AREA_IMAGE_SECTOR_SIZE    (BSP_FEATURE_FLASH_HP_CF_REGION1_BLOCK_SIZE) /* 32 KB */
 #define MCUBOOT_BOOT_MAX_ALIGN          (BSP_FEATURE_FLASH_HP_CF_WRITE_SIZE)
#else
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (BSP_FEATURE_FLASH_LP_CF_BLOCK_SIZE)         /* 2 KB */
#endif

#if (MCUBOOT_IMAGE_NUMBER == 1)
#define RM_MCUBOOT_LARGEST_SECTOR       (RM_MCUBOOT_PORT_CFG_PARTITION_SIZE)
#elif (MCUBOOT_IMAGE_NUMBER == 2)
 #define RM_MCUBOOT_LARGEST_SECTOR       (RM_MCUBOOT_PORT_CFG_S_PARTITION_SIZE > \
                                          RM_MCUBOOT_PORT_CFG_NS_PARTITION_SIZE ? \
                                          RM_MCUBOOT_PORT_CFG_S_PARTITION_SIZE : \
                                          RM_MCUBOOT_PORT_CFG_NS_PARTITION_SIZE)
#else                                  /* MCUBOOT_IMAGE_NUMBER > 2 */
 #error "Only MCUBOOT_IMAGE_NUMBER 1 and 2 are supported!"
#endif /* MCUBOOT_IMAGE_NUMBER */

/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS    ((RM_MCUBOOT_LARGEST_SECTOR) / \
                                       FLASH_AREA_IMAGE_SECTOR_SIZE)
#endif /* MCUBOOT_CONFIG_H_ */
