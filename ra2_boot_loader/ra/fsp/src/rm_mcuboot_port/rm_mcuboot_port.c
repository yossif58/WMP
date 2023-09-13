#include <stdio.h>
#include "bsp_api.h"
#include "rm_mcuboot_port.h"
#include "rm_mcuboot_port_cfg.h"
#include "bootutil/bootutil_log.h"
#include "flash_map_backend/flash_map_backend.h"
#include "flash_lp_ep.h" // Y.F. 7/12/22
#include "r_flash_lp_cfg.h"
#define UPGRADE_REQ_IS_WAITING 89
#if defined(__GNUC__) && !defined(__ARMCC_VERSION) && !defined(__clang_analyzer__)
 #define RM_MCUBOOT_PORT_NAKED_FUNCTION    __attribute__((naked, no_instrument_function, \
                                                          no_profile_instrument_function))
#else
 #define RM_MCUBOOT_PORT_NAKED_FUNCTION    __attribute__((naked))
#endif

static void start_app(uint32_t pc, uint32_t sp) RM_MCUBOOT_PORT_NAKED_FUNCTION;

/* This function jumps to the application image. */
static void start_app (uint32_t pc __attribute__((unused)), uint32_t sp __attribute__((unused))) {
    /* This is a naked/stackless function. Do not pass arguments to the inline assembly when the GCC compiler is
     * used. */
    __asm volatile (

        /* Set stack pointer. */
        "MSR     MSP, R1                         \n"
        "DSB                                     \n"
        "ISB                                     \n"

        /* Branch to application. */
        "BX      R0                              \n"
        );
}

/*******************************************************************************************************************//**
 * Cleans up resources used by bootloader port and branches to application image.
 **********************************************************************************************************************/
void RM_MCUBOOT_PORT_BootApp (struct boot_rsp * rsp) {
    BOOT_LOG_DBG("Starting Application Image\n");
    BOOT_LOG_DBG("Image Offset: 0x%x\n", (int) rsp->br_image_off);
    uint32_t UPGRADE_STATE=0;
   // R_FLASH_LP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    __disable_irq(); // y.f. 12/7/22 add
    memcpy (&UPGRADE_STATE, (unsigned char*) (__IO uint32_t*) FLASH_DF_BLOCK(0) ,4);
    if (UPGRADE_STATE == UPGRADE_REQ_IS_WAITING)
    {
        OverWrite_image_1();
    }

    /* Clean up internal flash driver. */
    flash_on_chip_cleanup();

    /* The vector table is located after the header. Only running from internal flash is supported now. */
    //   uint32_t vector_table = rsp->br_image_off + rsp->br_hdr->ih_hdr_size;
    uint32_t vector_table = 0x1900;

    uint32_t * app_image = (uint32_t *) vector_table;
    uint32_t   app_sp    = app_image[0];
    uint32_t   app_pc    = app_image[1];

    BOOT_LOG_DBG("Vector Table: 0x%x. PC=0x%x, SP=0x%x\n", (unsigned int) vector_table, (unsigned int) app_pc,
                 (unsigned int) app_sp);

    /* Set the applications vector table. */
    SCB->VTOR = (uint32_t) vector_table;

    /* Disable MSP monitoring. */
#if BSP_FEATURE_TZ_HAS_TRUSTZONE
    __set_MSPLIM(0);
#else
    R_MPU_SPMON->SP[0].CTL = 0;
#endif

    /* Set SP and branch to PC. */
    start_app(app_pc, app_sp);
}
