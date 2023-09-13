/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_agt.h"
#include "r_timer_api.h"
#include "r_flash_lp.h"
#include "r_flash_api.h"
#include "r_lpm.h"
#include "r_lpm_api.h"
#include "r_rtc.h"
#include "r_rtc_api.h"
#include "r_lvd.h"
#include "r_lvd_api.h"
#include "r_icu.h"
#include "r_external_irq_api.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "r_adc.h"
#include "r_adc_api.h"
#include "r_wdt.h"
#include "r_wdt_api.h"
FSP_HEADER
/** AGT Timer Instance */
extern const timer_instance_t Pulser_Timer0_Generic;

/** Access the AGT instance using these structures when calling API functions directly (::p_api is not used). */
extern agt_instance_ctrl_t Pulser_Timer0_Generic_ctrl;
extern const timer_cfg_t Pulser_Timer0_Generic_cfg;

#ifndef Pulser_timer_generic_callback
void Pulser_timer_generic_callback(timer_callback_args_t *p_args);
#endif
/* Flash on Flash LP Instance. */
extern const flash_instance_t g_flash;

/** Access the Flash LP instance using these structures when calling API functions directly (::p_api is not used). */
extern flash_lp_instance_ctrl_t g_flash_ctrl;
extern const flash_cfg_t g_flash_cfg;

#ifndef bgo_callback
void bgo_callback(flash_callback_args_t *p_args);
#endif
/** lpm Instance */
extern const lpm_instance_t Meter_g_lpm0;

/** Access the LPM instance using these structures when calling API functions directly (::p_api is not used). */
extern lpm_instance_ctrl_t Meter_g_lpm0_ctrl;
extern const lpm_cfg_t Meter_g_lpm0_cfg;
/* RTC Instance. */
extern const rtc_instance_t Yos1_RTC;

/** Access the RTC instance using these structures when calling API functions directly (::p_api is not used). */
extern rtc_instance_ctrl_t Yos1_RTC_ctrl;
extern const rtc_cfg_t Yos1_RTC_cfg;

#ifndef yos1_rtc_callback
void yos1_rtc_callback(rtc_callback_args_t *p_args);
#endif
/** LVD Instance */
extern const lvd_instance_t Battery_g_lvd;

/** Access the LVD instance using these structures when calling API functions directly (::p_api is not used). */
extern lvd_instance_ctrl_t Battery_g_lvd_ctrl;
extern const lvd_cfg_t Battery_g_lvd_cfg;

#ifndef Battery_low_callback
void Battery_low_callback(lvd_callback_args_t *p_args);
#endif
/** External IRQ on ICU Instance. */
extern const external_irq_instance_t Puls_g_external_irq0;

/** Access the ICU instance using these structures when calling API functions directly (::p_api is not used). */
extern icu_instance_ctrl_t Puls_g_external_irq0_ctrl;
extern const external_irq_cfg_t Puls_g_external_irq0_cfg;

#ifndef Pulse_int_callback
void Pulse_int_callback(external_irq_callback_args_t *p_args);
#endif
/** UART on SCI Instance. */
extern const uart_instance_t LTE_Uart;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t LTE_Uart_ctrl;
extern const uart_cfg_t LTE_Uart_cfg;
extern const sci_uart_extended_cfg_t LTE_Uart_cfg_extend;

#ifndef LTE_Uart_callback
void LTE_Uart_callback(uart_callback_args_t *p_args);
#endif
/** UART on SCI Instance. */
extern const uart_instance_t Debug_Uart1;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t Debug_Uart1_ctrl;
extern const uart_cfg_t Debug_Uart1_cfg;
extern const sci_uart_extended_cfg_t Debug_Uart1_cfg_extend;

#ifndef Debug_uart_callback
void Debug_uart_callback(uart_callback_args_t *p_args);
#endif
/** External IRQ on ICU Instance. */
extern const external_irq_instance_t magnet_detect;

/** Access the ICU instance using these structures when calling API functions directly (::p_api is not used). */
extern icu_instance_ctrl_t magnet_detect_ctrl;
extern const external_irq_cfg_t magnet_detect_cfg;

#ifndef magnet_detect_irq_callback
void magnet_detect_irq_callback(external_irq_callback_args_t *p_args);
#endif
/** ADC on ADC Instance. */
extern const adc_instance_t g_adc0_Vbat;

/** Access the ADC instance using these structures when calling API functions directly (::p_api is not used). */
extern adc_instance_ctrl_t g_adc0_Vbat_ctrl;
extern const adc_cfg_t g_adc0_Vbat_cfg;
extern const adc_channel_cfg_t g_adc0_Vbat_channel_cfg;

#ifndef Adc_Comp_callback
void Adc_Comp_callback(adc_callback_args_t *p_args);
#endif

#ifndef NULL
#define ADC_DMAC_CHANNELS_PER_BLOCK_NULL  1
#endif
/** WDT on WDT Instance. */
extern const wdt_instance_t g_wdt0;

/** Access the WDT instance using these structures when calling API functions directly (::p_api is not used). */
extern wdt_instance_ctrl_t g_wdt0_ctrl;
extern const wdt_cfg_t g_wdt0_cfg;

#ifndef NULL
void NULL(wdt_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
