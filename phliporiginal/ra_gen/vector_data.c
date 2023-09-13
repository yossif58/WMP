/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_uart_rxi_isr, /* SCI1 RXI (Received data full) */
            [1] = sci_uart_txi_isr, /* SCI1 TXI (Transmit data empty) */
            [2] = sci_uart_tei_isr, /* SCI1 TEI (Transmit end) */
            [3] = sci_uart_eri_isr, /* SCI1 ERI (Receive error) */
            [4] = adc_scan_end_isr, /* ADC0 SCAN END (A/D scan end interrupt) */
            [5] = r_icu_isr, /* ICU IRQ5 (External pin interrupt 5) */
            [6] = sci_uart_tei_isr, /* SCI0 TEI (Transmit end) */
            [7] = sci_uart_eri_isr, /* SCI0 ERI (Receive error) */
            [8] = r_icu_isr, /* ICU IRQ0 (External pin interrupt 0) */
            [9] = sci_uart_txi_isr, /* SCI0 TXI (Transmit data empty) */
            [10] = fcu_frdyi_isr, /* FCU FRDYI (Flash ready interrupt) */
            [11] = agt_int_isr, /* AGT0 INT (AGT interrupt) */
            [12] = sci_uart_rxi_isr, /* SCI0 RXI (Receive data full) */
            [13] = rtc_alarm_periodic_isr, /* RTC ALARM (Alarm interrupt) */
            [15] = rtc_carry_isr, /* RTC CARRY (Carry interrupt) */
            [16] = lvd_lvd_isr, /* LVD LVD1 (Voltage monitor 1 interrupt) */
        };
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [0] = BSP_PRV_IELS_ENUM(EVENT_SCI1_RXI), /* SCI1 RXI (Received data full) */
            [1] = BSP_PRV_IELS_ENUM(EVENT_SCI1_TXI), /* SCI1 TXI (Transmit data empty) */
            [2] = BSP_PRV_IELS_ENUM(EVENT_SCI1_TEI), /* SCI1 TEI (Transmit end) */
            [3] = BSP_PRV_IELS_ENUM(EVENT_SCI1_ERI), /* SCI1 ERI (Receive error) */
            [4] = BSP_PRV_IELS_ENUM(EVENT_ADC0_SCAN_END), /* ADC0 SCAN END (A/D scan end interrupt) */
            [5] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ5), /* ICU IRQ5 (External pin interrupt 5) */
            [6] = BSP_PRV_IELS_ENUM(EVENT_SCI0_TEI), /* SCI0 TEI (Transmit end) */
            [7] = BSP_PRV_IELS_ENUM(EVENT_SCI0_ERI), /* SCI0 ERI (Receive error) */
            [8] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ0), /* ICU IRQ0 (External pin interrupt 0) */
            [9] = BSP_PRV_IELS_ENUM(EVENT_SCI0_TXI), /* SCI0 TXI (Transmit data empty) */
            [10] = BSP_PRV_IELS_ENUM(EVENT_FCU_FRDYI), /* FCU FRDYI (Flash ready interrupt) */
            [11] = BSP_PRV_IELS_ENUM(EVENT_AGT0_INT), /* AGT0 INT (AGT interrupt) */
            [12] = BSP_PRV_IELS_ENUM(EVENT_SCI0_RXI), /* SCI0 RXI (Receive data full) */
            [13] = BSP_PRV_IELS_ENUM(EVENT_RTC_ALARM), /* RTC ALARM (Alarm interrupt) */
            [15] = BSP_PRV_IELS_ENUM(EVENT_RTC_CARRY), /* RTC CARRY (Carry interrupt) */
            [16] = BSP_PRV_IELS_ENUM(EVENT_LVD_LVD1), /* LVD LVD1 (Voltage monitor 1 interrupt) */
        };
        #endif
