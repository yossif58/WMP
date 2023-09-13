/* generated HAL source file - do not edit */
#include "hal_data.h"
flash_lp_instance_ctrl_t g_flash0_ctrl;
const flash_cfg_t g_flash0_cfg =
{ .data_flash_bgo = false, .p_callback = flash0_call_back, .p_context = NULL, .ipl = (3),
#if defined(VECTOR_NUMBER_FCU_FRDYI)
    .irq                 = VECTOR_NUMBER_FCU_FRDYI,
#else
  .irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const flash_instance_t g_flash0 =
{ .p_ctrl = &g_flash0_ctrl, .p_cfg = &g_flash0_cfg, .p_api = &g_flash_on_flash_lp };
void *const gp_mcuboot_flash_ctrl = &g_flash0_ctrl;
flash_cfg_t const *const gp_mcuboot_flash_cfg = &g_flash0_cfg;
flash_instance_t const *const gp_mcuboot_flash_instance = &g_flash0;
void g_hal_init(void)
{
    g_common_init ();
}
