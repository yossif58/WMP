/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "flash_map_backend/flash_map_backend.h"
#include "bootutil/bootutil_log.h"
#include "sysflash/sysflash.h"
#include "bsp_api.h"
#if BSP_FEATURE_FLASH_HP_VERSION > 0
 #include "r_flash_hp.h"
#else
 #include "r_flash_lp.h"
#endif

/* Definitions for different flash implementation. */
#if BSP_FEATURE_FLASH_HP_VERSION > 0
 #define RM_MCUBOOT_PORT_BLOCK_ALIGN              (BSP_FEATURE_FLASH_HP_CF_WRITE_SIZE)
 #define R_FLASH_Open                             R_FLASH_HP_Open
 #define R_FLASH_Write                            R_FLASH_HP_Write
 #define R_FLASH_Erase                            R_FLASH_HP_Erase
 #define R_FLASH_Close                            R_FLASH_HP_Close
#else
 #define RM_MCUBOOT_PORT_BLOCK_ALIGN              (BSP_FEATURE_FLASH_LP_CF_WRITE_SIZE)
 #define R_FLASH_Open                             R_FLASH_LP_Open
 #define R_FLASH_Write                            R_FLASH_LP_Write
 #define R_FLASH_Erase                            R_FLASH_LP_Erase
 #define R_FLASH_Close                            R_FLASH_LP_Close
#endif

/* Write buffering is not required in overwrite only mode (it is needed for overwrite only fast). */
#if defined(MCUBOOT_OVERWRITE_ONLY) && !defined(MCUBOOT_OVERWRITE_ONLY_FAST)
 #define RM_MCUBOOT_PORT_BUFFERED_WRITE_ENABLE    (0)
#else
 #define RM_MCUBOOT_PORT_BUFFERED_WRITE_ENABLE    (1)
#endif

/* Instance structure to use this module. */
extern void * const              gp_mcuboot_flash_ctrl;
extern flash_cfg_t const * const gp_mcuboot_flash_cfg;

#if RM_MCUBOOT_PORT_BUFFERED_WRITE_ENABLE
uint32_t g_rm_mcuboot_port_flash_write_ram[RM_MCUBOOT_PORT_BLOCK_ALIGN / sizeof(uint32_t)];
uint32_t g_current_block = UINT32_MAX;
#endif

/* Flash device name must be specified by target */

static const struct flash_area flash_map[] =
{
    {
        .fa_id        = FLASH_AREA_0_ID,
        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
        .fa_off       = FLASH_AREA_0_OFFSET,
        .fa_size      = FLASH_AREA_0_SIZE,
    },
    {
        .fa_id        = FLASH_AREA_2_ID,
        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
        .fa_off       = FLASH_AREA_2_OFFSET,
        .fa_size      = FLASH_AREA_2_SIZE,
    },
#if (MCUBOOT_IMAGE_NUMBER == 2)
    {
        .fa_id        = FLASH_AREA_1_ID,
        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
        .fa_off       = FLASH_AREA_1_OFFSET,
        .fa_size      = FLASH_AREA_1_SIZE,
    },
    {
        .fa_id        = FLASH_AREA_3_ID,
        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
        .fa_off       = FLASH_AREA_3_OFFSET,
        .fa_size      = FLASH_AREA_3_SIZE,
    },
#endif

#if !defined(MCUBOOT_OVERWRITE_ONLY) && !defined(MCUBOOT_DIRECT_XIP)

    /* Scratch region is only used in swap mode. */
    {
        .fa_id        = FLASH_AREA_SCRATCH_ID,
        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
        .fa_off       = FLASH_AREA_SCRATCH_OFFSET,
        .fa_size      = FLASH_AREA_SCRATCH_SIZE,
    },
#endif
};

static const struct flash_area * prv_lookup_flash_area(int id);

static const uint32_t flash_map_entry_num = sizeof(flash_map) / sizeof(struct flash_area);

static bool g_driver_open = false;

/*< Opens the area for use. id is one of the `fa_id`s */
int flash_area_open (uint8_t id, const struct flash_area ** area)
{
    /* Open the flash driver if it hasn't been opened yet. */
    if (!g_driver_open)
    {
        fsp_err_t err = R_FLASH_Open(gp_mcuboot_flash_ctrl, gp_mcuboot_flash_cfg);
        if (FSP_SUCCESS != err)
        {
            return -1;
        }

        g_driver_open = true;
    }

    BOOT_LOG_DBG("area %d", id);

    /* Copy the area's information to the provided pointer. */
    *area = prv_lookup_flash_area(id);
    assert(NULL != *area);

    /* Success. */
    return 0;
}

/*< Closes the area for use. */
void flash_area_close (const struct flash_area * area)
{
    FSP_PARAMETER_NOT_USED(area);

    /* Nothing to do. The flash module is closed before entering the application image. */
}

/*< Reads `len` bytes of flash memory at `off` to the buffer at `dst` */
int flash_area_read (const struct flash_area * area, uint32_t off, void * dst, uint32_t len)
{
#if RM_MCUBOOT_PORT_BUFFERED_WRITE_ENABLE

    /* If any buffered writes have not yet been written, write them before reading. */
    int ret = flash_on_chip_flush();
    assert(0 == ret);
    FSP_PARAMETER_NOT_USED(ret);
#endif

    /* Internal flash is memory mapped. Just read with memcpy. */
    BOOT_LOG_DBG("read area=%d, off=%#x, len=%#x", (unsigned int) area->fa_id, (unsigned int) off, (unsigned int) len);
    memcpy(dst, (void *) (area->fa_off + off), len);

    return 0;
}

#if RM_MCUBOOT_PORT_BUFFERED_WRITE_ENABLE

/*< Flushes internal flash buffer. */
int flash_on_chip_flush (void)
{
    /* If the write buffer has data, write it. */
    if (UINT32_MAX != g_current_block)
    {
        BOOT_LOG_DBG("write flush ram buffer, addr=%#x, len=%#x",
                     (unsigned int) g_current_block,
                     (unsigned int) RM_MCUBOOT_PORT_BLOCK_ALIGN);

        FSP_CRITICAL_SECTION_DEFINE;
        FSP_CRITICAL_SECTION_ENTER;
        int err = (int) R_FLASH_Write(gp_mcuboot_flash_ctrl,
                                      (uint32_t) &g_rm_mcuboot_port_flash_write_ram[0],
                                      g_current_block,
                                      RM_MCUBOOT_PORT_BLOCK_ALIGN);
        g_current_block = UINT32_MAX;
        FSP_CRITICAL_SECTION_EXIT;

        return err;
    }

    return 0;
}

#endif

/*< Writes `len` bytes of flash memory at `off` from the buffer at `src` */
int flash_area_write (const struct flash_area * area, uint32_t off, const void * src, uint32_t len)
{
    int err = 0;

#if RM_MCUBOOT_PORT_BUFFERED_WRITE_ENABLE

    /* If the write length is less than the minimum write size, buffer it. MCUboot always writes from an address
     * with the required alignment when the length is a multiple of the required alignment, so we are only checking
     * len here. */
    if (len % RM_MCUBOOT_PORT_BLOCK_ALIGN != 0)
    {
        /* Ensure all writes meet minimum alignment. */
        uint32_t aligned_address = (uint32_t) ((area->fa_off + off) & (uint32_t) ~(RM_MCUBOOT_PORT_BLOCK_ALIGN - 1));
        uint32_t offset          = (area->fa_off + off) % RM_MCUBOOT_PORT_BLOCK_ALIGN;
        if (g_current_block != aligned_address)
        {
            /* If the requested write block is different from the data in the buffer, flush the buffer before
             * buffering a new write block. */
            err = flash_on_chip_flush();
            memset(&g_rm_mcuboot_port_flash_write_ram[0], UINT8_MAX, RM_MCUBOOT_PORT_BLOCK_ALIGN);
            g_current_block = aligned_address;
        }
        else
        {
            /* If the requested write block is the same as the last requested write block, copy data into the
             * write buffer. */
        }

        memcpy(&g_rm_mcuboot_port_flash_write_ram[offset / sizeof(uint32_t)], src, len);
        BOOT_LOG_DBG("write buffer=%d, off=%#x, len=%#x",
                     (unsigned int) area->fa_id,
                     (unsigned int) off,
                     (unsigned int) len);
    }
    else
#endif
    {
        /* If the write is a multiple of minimum write size, write it to flash. */
        BOOT_LOG_DBG("write area=%d, off=%#x, len=%#x",
                     (unsigned int) area->fa_id,
                     (unsigned int) off,
                     (unsigned int) len);

        uint32_t write_addr = area->fa_off + off;

        FSP_CRITICAL_SECTION_DEFINE;
        FSP_CRITICAL_SECTION_ENTER;
        err = (int) R_FLASH_Write(gp_mcuboot_flash_ctrl, (uint32_t) src, write_addr, len);
        FSP_CRITICAL_SECTION_EXIT;
    }

    return err;
}

/*< Erases `len` bytes of flash memory at `off` */
int flash_area_erase (const struct flash_area * area, uint32_t off, uint32_t len)
{
    /* No need to flush the write buffer here since it does not make sense to write to a block just before erasing it. */

    BOOT_LOG_DBG("erase area=%d, off=%#x, len=%#x", (unsigned int) area->fa_id, (unsigned int) off, (unsigned int) len);

    fsp_err_t err              = FSP_SUCCESS;
    uint32_t  erase_address    = area->fa_off + off;
    uint32_t  sectors_to_erase = 0;
    uint32_t  sector_size;

    /* Erase, accounting for block sizes. */

#if BSP_FEATURE_FLASH_HP_VERSION > 0
    uint32_t deleted_len = 0;
    while (deleted_len < len)
    {
        if (erase_address < BSP_FEATURE_FLASH_HP_CF_REGION0_SIZE)
        {
            sector_size = BSP_FEATURE_FLASH_HP_CF_REGION0_BLOCK_SIZE;
        }
        else
        {
            sector_size = BSP_FEATURE_FLASH_HP_CF_REGION1_BLOCK_SIZE;
        }

        sectors_to_erase++;
        deleted_len += sector_size;
    }

#else
    sector_size      = BSP_FEATURE_FLASH_LP_CF_BLOCK_SIZE;
    sectors_to_erase = len / sector_size;
#endif

    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;
    err = R_FLASH_Erase(gp_mcuboot_flash_ctrl, erase_address, sectors_to_erase);
    FSP_CRITICAL_SECTION_EXIT;
    assert(FSP_SUCCESS == err);

    return (int) err;
}

/*< Returns this `flash_area`s alignment */
uint8_t flash_area_align (const struct flash_area * area)
{
    (void) area;

    return RM_MCUBOOT_PORT_BLOCK_ALIGN;
}

/*< Returns the `flash_area` for a given `id` */
static const struct flash_area * prv_lookup_flash_area (int id)
{
    for (uint32_t i = 0; i < flash_map_entry_num; i++)
    {
        const struct flash_area * p_area = &flash_map[i];
        if (id == p_area->fa_id)
        {
            return p_area;
        }
    }

    return NULL;
}

#ifdef MCUBOOT_USE_FLASH_AREA_GET_SECTORS

/*< Given flash area ID, return info about sectors within the area. */
int flash_area_get_sectors (int fa_id, uint32_t * count, struct flash_sector * sectors)
{
    const struct flash_area * fa = prv_lookup_flash_area(fa_id);
    if (fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH)
    {
        return -1;
    }

    /* All sectors are treated as the same size for internal flash. */
 #if BSP_FEATURE_FLASH_HP_VERSION > 0
    const size_t sector_size = BSP_FEATURE_FLASH_HP_CF_REGION1_BLOCK_SIZE;
 #else
    const size_t sector_size = BSP_FEATURE_FLASH_LP_CF_BLOCK_SIZE;
 #endif
    uint32_t total_count = 0;
    for (size_t off = 0; off < fa->fa_size; off += sector_size)
    {
        /* Note: Offset here is relative to flash area, not device */
        sectors[total_count].fs_off  = off;
        sectors[total_count].fs_size = sector_size;
        total_count++;
    }

    *count = total_count;

    return 0;
}

#endif

/*< Returns the `fa_id` for slot, where slot is 0 (primary) or 1 (secondary).
 *  `image_index` (0 or 1) is the index of the image. Image index is
 *  relevant only when multi-image support support is enabled */
int flash_area_id_from_multi_image_slot (int image_index, int slot)
{
    switch (slot)
    {
        case 0:
        {
            return FLASH_AREA_IMAGE_PRIMARY(image_index);
        }

        case 1:
        {
            return FLASH_AREA_IMAGE_SECONDARY(image_index);
        }

#if MCUBOOT_SWAP_USING_SCRATCH
        case 2:
        {
            return FLASH_AREA_IMAGE_SCRATCH;
        }
#endif
    }

    return -1;                         /* flash_area_open will fail on that */
}

/*< Returns the slot (0 for primary or 1 for secondary), for the supplied
 *  `image_index` and `area_id`. `area_id` is unique and is represented by
 *  `fa_id` in the `flash_area` struct. */
int flash_area_id_to_multi_image_slot (int image_index, int area_id)
{
    if (area_id == FLASH_AREA_IMAGE_PRIMARY(image_index))
    {
        return 0;
    }

    if (area_id == FLASH_AREA_IMAGE_SECONDARY(image_index))
    {
        return 1;
    }

    BOOT_LOG_ERR("invalid flash area ID");

    return -1;
}

/*< What is value is read from erased flash bytes. */
uint8_t flash_area_erased_val (const struct flash_area * area)
{
    (void) area;

    return UINT8_MAX;
}

/*< Closes the area for use. id is one of the `fa_id`s */
int flash_on_chip_cleanup (void)
{
    if (g_driver_open)
    {
#if RM_MCUBOOT_PORT_BUFFERED_WRITE_ENABLE

        /* Flush the write buffer. */
        int ret = flash_on_chip_flush();
        assert(0 == ret);
        FSP_PARAMETER_NOT_USED(ret);
#endif

        /* Close the flash driver. */
        fsp_err_t err = R_FLASH_Close(gp_mcuboot_flash_ctrl);
        if (FSP_SUCCESS != err)
        {
            return -1;
        }

        g_driver_open = false;
    }

    return 0;
}

/* Unused, required to link with AC6. */
int flash_area_id_from_image_slot (int slot)
{
    FSP_PARAMETER_NOT_USED(slot);

    return -1;
}

#if defined(__ARMCC_VERSION)

/* Assign region addresses to pointers so that AC6 includes symbols that can be used to determine the
 * start addresses of Secure, Non-secure and Non-secure Callable regions. */
uint32_t const __bl_FLASH_IMAGE_START BSP_PLACE_IN_SECTION(".bl_boundary.bl_flash_image_start") = 0;
uint32_t const __bl_FLASH_IMAGE_END   BSP_PLACE_IN_SECTION(".bl_boundary.bl_flash_image_end")   = 0;

 #if RM_MCUBOOT_PORT_CFG_NS_PARTITION_SIZE > 0

uint32_t const __bl_FLASH_NS_START       BSP_PLACE_IN_SECTION(".bl_boundary.bl_flash_ns_start")       = 0;
uint32_t const __bl_FLASH_NSC_START      BSP_PLACE_IN_SECTION(".bl_boundary.bl_flash_nsc_start")      = 0;
uint32_t const __bl_FLASH_NS_IMAGE_START BSP_PLACE_IN_SECTION(".bl_boundary.bl_flash_ns_image_start") = 0;
uint32_t const __bl_RAM_NS_START         BSP_PLACE_IN_SECTION(".bl_boundary.bl_ram_ns_start")         = 0;
uint32_t const __bl_RAM_NSC_START        BSP_PLACE_IN_SECTION(".bl_boundary.bl_ram_nsc_start")        = 0;

  #if (MCUBOOT_IMAGE_NUMBER == 2)

uint32_t const __bln_FLASH_IMAGE_START BSP_PLACE_IN_SECTION(".bl_boundary.bln_flash_image_start") = 0;
uint32_t const __bln_FLASH_IMAGE_END   BSP_PLACE_IN_SECTION(".bl_boundary.bln_flash_image_end")   = 0;

  #endif

 #endif

/* Included to resolve a linker error. */
BSP_WEAK_REFERENCE void exit (int x)
{
    FSP_PARAMETER_NOT_USED(x);
    while (1)
    {
        ;
    }
}

#endif
