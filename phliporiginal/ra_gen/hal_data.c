/* generated HAL source file - do not edit */
#include "hal_data.h"
/* Macros to tie dynamic ELC links to ADC_TRIGGER_SYNC_ELC option in adc_trigger_t. */
#define ADC_TRIGGER_ADC0        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC0_B      ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1_B      ADC_TRIGGER_SYNC_ELC
agt_instance_ctrl_t Pulser_Timer0_Generic_ctrl;
const agt_extended_cfg_t Pulser_Timer0_Generic_extend =
{ .count_source = AGT_CLOCK_PCLKB,
  .agto = AGT_PIN_CFG_DISABLED,
  .agtoa = AGT_PIN_CFG_DISABLED,
  .agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED,
  .agtio_filter = AGT_AGTIO_FILTER_NONE,
  .enable_pin = AGT_ENABLE_PIN_NOT_USED,
  .trigger_edge = AGT_TRIGGER_EDGE_RISING, };
const timer_cfg_t Pulser_Timer0_Generic_cfg =
{ .mode = TIMER_MODE_PERIODIC,
/* Actual period: 0.01 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x7530,
  .duty_cycle_counts = 0x3a98, .source_div = (timer_source_div_t) 3, .channel = 0, .p_callback =
          Pulser_timer_generic_callback,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = &NULL,
#endif
  .p_extend = &Pulser_Timer0_Generic_extend,
  .cycle_end_ipl = (2),
#if defined(VECTOR_NUMBER_AGT0_INT)
    .cycle_end_irq       = VECTOR_NUMBER_AGT0_INT,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t Pulser_Timer0_Generic =
{ .p_ctrl = &Pulser_Timer0_Generic_ctrl, .p_cfg = &Pulser_Timer0_Generic_cfg, .p_api = &g_timer_on_agt };
flash_lp_instance_ctrl_t g_flash_ctrl;
const flash_cfg_t g_flash_cfg =
{ .data_flash_bgo = true, .p_callback = bgo_callback, .p_context = NULL, .ipl = (3),
#if defined(VECTOR_NUMBER_FCU_FRDYI)
    .irq                 = VECTOR_NUMBER_FCU_FRDYI,
#else
  .irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const flash_instance_t g_flash =
{ .p_ctrl = &g_flash_ctrl, .p_cfg = &g_flash_cfg, .p_api = &g_flash_on_flash_lp };
lpm_instance_ctrl_t Meter_g_lpm0_ctrl;

const lpm_cfg_t Meter_g_lpm0_cfg =
{ .low_power_mode = LPM_MODE_STANDBY,
  .snooze_cancel_sources = LPM_SNOOZE_CANCEL_SOURCE_NONE,
  .standby_wake_sources = LPM_STANDBY_WAKE_SOURCE_IRQ0 | LPM_STANDBY_WAKE_SOURCE_IRQ5 | LPM_STANDBY_WAKE_SOURCE_RTCALM
          | (lpm_standby_wake_source_t) 0,
  .snooze_request_source = LPM_SNOOZE_REQUEST_RXD0_FALLING,
  .snooze_end_sources = (lpm_snooze_end_t) 0,
  .dtc_state_in_snooze = LPM_SNOOZE_DTC_DISABLE,
#if BSP_FEATURE_LPM_HAS_SBYCR_OPE
    .output_port_enable         = 0,
#endif
#if BSP_FEATURE_LPM_HAS_DEEP_STANDBY
    .io_port_state              = 0,
    .power_supply_state         = 0,
    .deep_standby_cancel_source =  (lpm_deep_standby_cancel_source_t) 0,
    .deep_standby_cancel_edge   =  (lpm_deep_standby_cancel_edge_t) 0,
#endif
  .p_extend = NULL, };

const lpm_instance_t Meter_g_lpm0 =
{ .p_api = &g_lpm_on_lpm, .p_ctrl = &Meter_g_lpm0_ctrl, .p_cfg = &Meter_g_lpm0_cfg };
rtc_instance_ctrl_t Yos1_RTC_ctrl;
const rtc_error_adjustment_cfg_t Yos1_RTC_err_cfg =
{ .adjustment_mode = RTC_ERROR_ADJUSTMENT_MODE_AUTOMATIC,
  .adjustment_period = RTC_ERROR_ADJUSTMENT_PERIOD_10_SECOND,
  .adjustment_type = RTC_ERROR_ADJUSTMENT_NONE,
  .adjustment_value = 10, };
const rtc_cfg_t Yos1_RTC_cfg =
{ .clock_source = RTC_CLOCK_SOURCE_SUBCLK, .freq_compare_value_loco = 254, .p_err_cfg = &Yos1_RTC_err_cfg, .p_callback =
          yos1_rtc_callback,
  .p_context = NULL, .alarm_ipl = (2), .periodic_ipl = (BSP_IRQ_DISABLED), .carry_ipl = (2),
#if defined(VECTOR_NUMBER_RTC_ALARM)
    .alarm_irq               = VECTOR_NUMBER_RTC_ALARM,
#else
  .alarm_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_RTC_PERIOD)
    .periodic_irq            = VECTOR_NUMBER_RTC_PERIOD,
#else
  .periodic_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_RTC_CARRY)
    .carry_irq               = VECTOR_NUMBER_RTC_CARRY,
#else
  .carry_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const rtc_instance_t Yos1_RTC =
{ .p_ctrl = &Yos1_RTC_ctrl, .p_cfg = &Yos1_RTC_cfg, .p_api = &g_rtc_on_rtc };
lvd_instance_ctrl_t Battery_g_lvd_ctrl;
/* Configuration structure for Battery_g_lvd */
const lvd_cfg_t Battery_g_lvd_cfg =
        {
        /** LVD settings */
        .monitor_number = 1,
          .voltage_threshold = LVD_THRESHOLD_MONITOR_1_LEVEL_3_10V, .detection_response = LVD_RESPONSE_INTERRUPT, .voltage_slope =
                  LVD_VOLTAGE_SLOPE_FALLING,
          .negation_delay = LVD_NEGATION_DELAY_FROM_RESET, .sample_clock_divisor = LVD_SAMPLE_CLOCK_DISABLED,
#if defined(VECTOR_NUMBER_LVD_LVD1)
    .irq                          = VECTOR_NUMBER_LVD_LVD1,
#else
          .irq = FSP_INVALID_VECTOR,
#endif
          .monitor_ipl = (3),
          .p_callback = Battery_low_callback, .p_context = NULL, .p_extend = NULL, };
const lvd_instance_t Battery_g_lvd =
{ .p_api = &g_lvd_on_lvd, .p_cfg = &Battery_g_lvd_cfg, .p_ctrl = &Battery_g_lvd_ctrl, };
icu_instance_ctrl_t Puls_g_external_irq0_ctrl;
const external_irq_cfg_t Puls_g_external_irq0_cfg =
{ .channel = 5,
  .trigger = EXTERNAL_IRQ_TRIG_RISING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .p_callback = Pulse_int_callback,
  .p_context = NULL,
  .p_extend = NULL,
  .ipl = (1),
#if defined(VECTOR_NUMBER_ICU_IRQ5)
    .irq                 = VECTOR_NUMBER_ICU_IRQ5,
#else
  .irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const external_irq_instance_t Puls_g_external_irq0 =
{ .p_ctrl = &Puls_g_external_irq0_ctrl, .p_cfg = &Puls_g_external_irq0_cfg, .p_api = &g_external_irq_on_icu };
sci_uart_instance_ctrl_t LTE_Uart_ctrl;

baud_setting_t LTE_Uart_baud_setting =
{
/* Baud rate calculated with 0.160% error. */.abcse = 0,
  .abcs = 0, .bgdm = 1, .cks = 0, .brr = 12, .mddr = (uint8_t) 256, .brme = false };

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t LTE_Uart_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT, .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
          SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &LTE_Uart_baud_setting, .flow_control =
          SCI_UART_FLOW_CONTROL_RTS,
#if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
  .flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
        };

/** UART interface configuration */
const uart_cfg_t LTE_Uart_cfg =
{ .channel = 0, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          LTE_Uart_callback,
  .p_context = NULL, .p_extend = &LTE_Uart_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (2),
  .txi_ipl = (2), .tei_ipl = (2), .eri_ipl = (2),
#if defined(VECTOR_NUMBER_SCI0_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI0_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI0_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI0_TEI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI0_ERI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t LTE_Uart =
{ .p_ctrl = &LTE_Uart_ctrl, .p_cfg = &LTE_Uart_cfg, .p_api = &g_uart_on_sci };
sci_uart_instance_ctrl_t Debug_Uart1_ctrl;

baud_setting_t Debug_Uart1_baud_setting =
{
/* Baud rate calculated with 0.160% error. */.abcse = 0,
  .abcs = 0, .bgdm = 1, .cks = 0, .brr = 77, .mddr = (uint8_t) 256, .brme = false };

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t Debug_Uart1_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT, .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
          SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &Debug_Uart1_baud_setting, .flow_control =
          SCI_UART_FLOW_CONTROL_RTS,
#if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
  .flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
        };

/** UART interface configuration */
const uart_cfg_t Debug_Uart1_cfg =
{ .channel = 1, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          Debug_uart_callback,
  .p_context = NULL, .p_extend = &Debug_Uart1_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (2),
  .txi_ipl = (2), .tei_ipl = (2), .eri_ipl = (2),
#if defined(VECTOR_NUMBER_SCI1_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI1_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI1_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI1_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI1_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI1_TEI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI1_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI1_ERI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t Debug_Uart1 =
{ .p_ctrl = &Debug_Uart1_ctrl, .p_cfg = &Debug_Uart1_cfg, .p_api = &g_uart_on_sci };
icu_instance_ctrl_t magnet_detect_ctrl;
const external_irq_cfg_t magnet_detect_cfg =
{ .channel = 0,
  .trigger = EXTERNAL_IRQ_TRIG_RISING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .p_callback = magnet_detect_irq_callback,
  .p_context = NULL,
  .p_extend = NULL,
  .ipl = (2),
#if defined(VECTOR_NUMBER_ICU_IRQ0)
    .irq                 = VECTOR_NUMBER_ICU_IRQ0,
#else
  .irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const external_irq_instance_t magnet_detect =
{ .p_ctrl = &magnet_detect_ctrl, .p_cfg = &magnet_detect_cfg, .p_api = &g_external_irq_on_icu };
adc_instance_ctrl_t g_adc0_Vbat_ctrl;
const adc_extended_cfg_t g_adc0_Vbat_cfg_extend =
{ .add_average_count = ADC_ADD_OFF,
  .clearing = ADC_CLEAR_AFTER_READ_ON,
  .trigger_group_b = ADC_TRIGGER_SYNC_ELC,
  .double_trigger_mode = ADC_DOUBLE_TRIGGER_DISABLED,
  .adc_vref_control = ADC_VREF_CONTROL_AVCC0_AVSS0,
  .enable_adbuf = 0, };
const adc_cfg_t g_adc0_Vbat_cfg =
{ .unit = 0, .mode = ADC_MODE_SINGLE_SCAN, .resolution = ADC_RESOLUTION_12_BIT, .alignment =
          (adc_alignment_t) ADC_ALIGNMENT_RIGHT,
  .trigger = ADC_TRIGGER_SOFTWARE, .p_callback = Adc_Comp_callback, .p_context = NULL, .p_extend =
          &g_adc0_Vbat_cfg_extend,
#if defined(VECTOR_NUMBER_ADC0_SCAN_END)
    .scan_end_irq        = VECTOR_NUMBER_ADC0_SCAN_END,
#else
  .scan_end_irq = FSP_INVALID_VECTOR,
#endif
  .scan_end_ipl = (2),
#if defined(VECTOR_NUMBER_ADC0_SCAN_END_B)
    .scan_end_b_irq      = VECTOR_NUMBER_ADC0_SCAN_END_B,
#else
  .scan_end_b_irq = FSP_INVALID_VECTOR,
#endif
  .scan_end_b_ipl = (BSP_IRQ_DISABLED), };
const adc_channel_cfg_t g_adc0_Vbat_channel_cfg =
{ .scan_mask = ADC_MASK_CHANNEL_0 | 0, .scan_mask_group_b = 0, .priority_group_a = ADC_GROUP_A_PRIORITY_OFF, .add_mask =
          0,
  .sample_hold_mask = 0, .sample_hold_states = 24, };
/* Instance structure to use this module. */
const adc_instance_t g_adc0_Vbat =
{ .p_ctrl = &g_adc0_Vbat_ctrl, .p_cfg = &g_adc0_Vbat_cfg, .p_channel_cfg = &g_adc0_Vbat_channel_cfg, .p_api =
          &g_adc_on_adc };
wdt_instance_ctrl_t g_wdt0_ctrl;

const wdt_cfg_t g_wdt0_cfg =
{ .timeout = WDT_TIMEOUT_1024,
  .clock_division = WDT_CLOCK_DIVISION_8192,
  .window_start = WDT_WINDOW_START_100,
  .window_end = WDT_WINDOW_END_0,
  .reset_control = WDT_RESET_CONTROL_RESET,
  .stop_control = WDT_STOP_CONTROL_ENABLE,
  .p_callback = NULL, };

/* Instance structure to use this module. */
const wdt_instance_t g_wdt0 =
{ .p_ctrl = &g_wdt0_ctrl, .p_cfg = &g_wdt0_cfg, .p_api = &g_wdt_on_wdt };
void g_hal_init(void)
{
    g_common_init ();
}
