#include "hal_data.h"
#include "main.h"
#include "flash_lp_ep.h"
FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER
RepeaterConfigStruct RepeaterConfig;
rtc_time_t LastPulseTime;
bool IncPulseFlag = false;
bool toggle = false;
void ToggleLeds(void);
void SetLed(ioport_port_pin_t pin_num, bool STATE);
void SendData2Modem(uint8_t *DataIn, uint16_t Len);
void HourProcess(int hour,int year,int month,int day);
void modemProcess(void);
void RTC_Alarm_Set(void);
void ModemSequence(ModemModeSequence ModeSeq);
//void DelayYos(uint32_t Timems);
void EnterDefaultConfig(void);
void EnterDefaultTime(void);
void ResetConsumptionData(int32_t consumption);
void StartWMP_lpm(void);
uint32_t Start_ADC_VBAT(void);
uint16_t TmpHourConsmption; // y.f. 18/5/22

bool MagnetDetect = false;
uint32_t Vbat_val;
//uint32_t UploadTime;
//static int8_t min_counter = 0; // y.f. to be removed for test only
extern void Gsm_Handler(uint8_t * pDdataIn, uint32_t Len , EventType Event);
extern fsp_err_t Uarts_Initialize(void);
extern fsp_err_t Uarts_Close(void);
extern int8_t tmpbufout[];  // to be removed
extern fsp_err_t flash_lp_init(void);
extern uint8_t GsmCompleateStatus(void);
extern void ClrGsmCompleate(void);
extern void Gsm_Init(void);
extern bool FLashCheck(void);
extern uint32_t TimingDelay;
extern uint8_t Event_status[];
uint8_t modem_seq_state = 0;
uint32_t conttemp = 0;
bool ready_to_read = false;
bool ready_to_send_daily=true;
bool ready_to_update_daily=true;
const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;
const uint32_t delay = 5; // 10 sec y.f.
DailyData DailyConsumption[MAX_LOGGER_DAYS]; // count will increase if external flash is avalable
rtc_time_t CurrentTime;
bool RTC_Event = false;
bool write_internal_config_flag = false;
bool write_internal_logger_flag = false;
bool powerup_start = true;
#ifdef DEBUG_UART_MODE3
    char  str1[20];
#endif
void ClrLeds(void);

/* Temporary buffer to save data from receive buffer for further processing */
extern uint8_t g_LTE_temp_buffer[] ;
/* Counter to update g_temp_buffer index */
//extern uint32_t g_LTE_counter_var;
//long long freeztotalconsumption;

extern bool g_LTE_data_received_flag;
extern bool g_LTE_time_out_flag;

bool LtuGSM_Timeout = false;
uint32_t Ltutmpindexin;
uint8_t Ltutemputartin[MAX_UART_SIZE];
uint32_t Ltutmpindexout;
uint8_t Ltutemputartout[MAX_UART_SIZE];
#define LPM_CLOCK_LOCO                       2 // The local 32k
#define LPM_CLOCK_SUBCLOCK                   4 // The subclock oscillator.
#define LPM_CLOCK_HOCO                       0 // The high speed on chip oscillator.
extern void testmessage_in(void);
uint32_t upgradestate;
//uint32_t pulser_id              BSP_PLACE_IN_SECTION(".wmp_pulser_id*") =        22000004;              // y.f. 25/12/22 add meter id in constant flash code
uint32_t const pulser_id             =        22789100;              // y.f. 25/12/22 add meter id in constant flash code
uint32_t firmware_date          BSP_PLACE_IN_SECTION(".wmp_manufacture_date*") = 0x230105;                // y.f. 27/12/22 add frimware manufacture date
uint32_t CurrentFirmwareVersion BSP_PLACE_IN_SECTION(".wmp_firmware_version*") = 104;

void hal_entry(void)
{
    fsp_err_t status;
    lvd_status_t p_lvd_status;
  //  adc_status_t *p_status;

  R_BSP_SoftwareDelay (3000, bsp_delay_units); // delay 1 sec


//#define TEST_CURRENT
#ifdef TEST_CURRENT
    status = R_RTC_Open (&Yos1_RTC_ctrl, &Yos1_RTC_cfg);
    EnterDefaultTime ();

    while (1)
    {
         status = R_RTC_CalendarTimeGet (&Yos1_RTC_ctrl, &CurrentTime);
         R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);
         R_BSP_PinAccessEnable ();
         /* Write to this pin */
         R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
         R_BSP_PinWrite ((bsp_io_port_pin_t) LTE_VCC_ENABLE, BSP_IO_LEVEL_LOW); // Power off ltu
         R_BSP_PinWrite ((bsp_io_port_pin_t) WAKE_UP_LTE, BSP_IO_LEVEL_LOW);
         R_BSP_PinAccessDisable ();

         SetLed (PULSER_LED1, false);
         SetLed (PULSER_LED2, false);
 //        setsubclock();
         RTC_Alarm_Set();
         status = R_LPM_Open (&Meter_g_lpm0_ctrl, &Meter_g_lpm0_cfg);
         R_LPM_LowPowerModeEnter(&Meter_g_lpm0_ctrl);
//         bsp_clock_init();
         //sethococlk();
         SetLed (PULSER_LED1, true);
         SetLed (PULSER_LED2, true);
         R_BSP_SoftwareDelay (2000, bsp_delay_units); // delay 1 sec





#if 0
        status = R_IOPORT_Close(&g_ioport_ctrl);

        R_SYSTEM->SCKSCR = LPM_CLOCK_LOCO;
        status = R_LPM_Open (&Meter_g_lpm0_ctrl, &Meter_g_lpm0_cfg);
 //       R_SYSTEM->SCKSCR = LPM_CLOCK_LOCO;
        R_LPM_LowPowerModeEnter(&Meter_g_lpm0_ctrl);
#endif

    }
#endif

    status = R_LVD_Open (&Battery_g_lvd_ctrl, &Battery_g_lvd_cfg);

    status = R_ICU_ExternalIrqOpen (&magnet_detect_ctrl, &magnet_detect_cfg);
    status = R_ICU_ExternalIrqEnable (&magnet_detect_ctrl);
    status = R_ICU_ExternalIrqOpen (&Puls_g_external_irq0_ctrl, &Puls_g_external_irq0_cfg);
    status = R_ICU_ExternalIrqEnable (&Puls_g_external_irq0_ctrl);
    status = R_AGT_Open (&Pulser_Timer0_Generic_ctrl, &Pulser_Timer0_Generic_cfg);
    status = R_AGT_Enable (&Pulser_Timer0_Generic_ctrl);
    Uarts_Initialize (); // Init both uarts DEBUG & LTE
    flash_lp_init ();
    __enable_irq();
   // upgradestate = UPGRADE_REQ_IS_WAITING;
  //  WriteBootStateInFlash(&upgradestate); // for test only to be removed 22/12/22
    ResetUpgradeState();  // y.f 12/11/22
  //   RM_MCUBOOT_PORT_BootApp(); // for test only to be removed 11/12/22
  //  __disable_irq();
  //  testmessage_in(); // 20.10.22 y.f temp test message to be removed !!!!!!!!!
    __enable_irq();
    if (FLashCheck () == false)
    { //first time or flash corrupted
        EnterDefaultConfig ();
        ResetConsumptionData(0);
        WriteInternalFlashConfig(0,LOGGER_DATA); // store the configuration
    }
    else
    {
        GetDailyLoggerALLFromFlash ();
    }
    char strwmp[50];
    sprintf (strwmp, "Debug Port version %d.%02d water meter pulser%d\r\n", CurrentFirmwareVersion/100,CurrentFirmwareVersion%100,RepeaterConfig.Moduladdress);
    status = R_SCI_UART_Write (&Debug_Uart1_ctrl, strwmp, 52);

    // y.f. 30/8/2022 enter initalizaion every powerup
   // RepeaterConfig.Moduladdress = pulser_id;
 //   RepeaterConfig.DeviceInitalizaion=0;
  status = R_RTC_Open (&Yos1_RTC_ctrl, &Yos1_RTC_cfg);
  EnterDefaultTime ();
  status = R_RTC_CalendarTimeGet (&Yos1_RTC_ctrl, &CurrentTime);

  //  status = R_LPM_Open (&Meter_g_lpm0_ctrl, &Meter_g_lpm0_cfg);

    status = R_ADC_Open (&g_adc0_Vbat_ctrl, &g_adc0_Vbat_cfg);

    R_BSP_SoftwareDelay (2000, bsp_delay_units); // delay 1 sec
    RTC_Alarm_Set();
    // TEMP FOR DEBUG TOM BE REMOVED

    /* Disable ioport before going to LPM mode*/
//    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_lpm_cfg)
  //  R_LPM_LowPowerModeEnter(&Meter_g_lpm0_ctrl);

  //  SetLed (PULSER_LED1, true);
  //  SetLed (PULSER_LED2, true);
    status = R_CGC_Open(&g_cgc0_ctrl, &g_cgc0_cfg);
    powerup_start = true;
    __disable_irq();
  //   flash_lp_init ();
//     testmessage_in(); // 20.10.22 y.f temp test message to be removed !!!!!!!!!
     __enable_irq();


    while (1)
    {
           status = R_AGT_Start (&Pulser_Timer0_Generic_ctrl);
           status = R_RTC_CalendarTimeGet (&Yos1_RTC_ctrl, &CurrentTime);
           status = R_ADC_Open (&g_adc0_Vbat_ctrl, &g_adc0_Vbat_cfg);
           status = R_LVD_StatusGet (&Battery_g_lvd_ctrl, &p_lvd_status);
           if (p_lvd_status.current_state == 0)
           {
#ifdef DEBUG_UART_MODE
                R_SCI_UART_Write (&Debug_Uart1_ctrl, "Battery Low Detect\r\n", 15);
                R_BSP_SoftwareDelay (100, bsp_delay_units); // delay 1 sec temp to be removed
#endif
                Event_status[0] |= LOW_BATTERY_DETECT;
           }

            if ((MagnetDetect)|| (powerup_start))
            {
               //  MagnetDetect = false;
               //   powerup_start = false y.f. 13/12/2022 removed to clear only after MODEM_PROVIDER_STATE
                 Event_status[0] |= MAGNET_DETECT;
 #ifdef DEBUG_UART_MODE
                 R_SCI_UART_Write (&Debug_Uart1_ctrl, "Magnet Detect\r\n", 15);
                 R_BSP_SoftwareDelay (100, bsp_delay_units); // delay 1 sec temp to be removed
 #endif
                 modemProcess(); // start connection to server when magnet detect
     //            MagnetDetect = false;
            }
            if (RTC_Event) // 8.8.22 y.f.
 //           if (CurrentTime.tm_min % 15 == 0) // start  in midnight
            {
                RTC_Event = false;
                Vbat_val = Start_ADC_VBAT ();
                HourProcess (CurrentTime.tm_hour,CurrentTime.tm_year,CurrentTime.tm_mon,CurrentTime.tm_mday);
            }
            if  (write_internal_config_flag == true) // 14/6/22 temp use flag instead of direct write internal
            {
                write_internal_config_flag = false;
                WriteInternalFlashConfig(0,CONFIG_DATA); // store the configuration
            }
            if  (write_internal_logger_flag == true) // 14/6/22 temp use flag instead of direct write internal
             {
                write_internal_logger_flag = false;
                WriteInternalFlashConfig(0,LOGGER_DATA); // store the configuration
             }

            StartWMP_lpm();
    }
 }

void HourProcess(int hour,int year,int month,int day)
{
    // enter to flash
 //   char str[10];
    int tmp_hour;
    if (hour==0) tmp_hour = 23;
    else
        tmp_hour = hour-1;
    RepeaterConfig.Consumption.HourConsumption[tmp_hour] = TmpHourConsmption;
    R_ICU_ExternalIrqDisable (&Puls_g_external_irq0_ctrl);
    TmpHourConsmption = 0; // This parameters is modified in pulse isr
    R_ICU_ExternalIrqEnable (&Puls_g_external_irq0_ctrl);
    WriteInternalFlashConfig(0,CONFIG_DATA); // store the configuration
    if ((hour == 0) && (ready_to_update_daily))
    {
        ready_to_update_daily = false;
        RepeaterConfig.DailyLoggerIndex = (int16_t)(RepeaterConfig.DailyLoggerIndex+1) %MAX_LOGGER_DAYS;
        WriteInternalFlashConfig(0,LOGGER_DATA); // store the logger
        // enter time for next day in repeater config
        RepeaterConfig.Consumption.Day=(int8_t)day;
        RepeaterConfig.Consumption.Month=(int8_t)month;
        RepeaterConfig.Consumption.Year=(int8_t)year;
        RepeaterConfig.freeztotalconsumption=RepeaterConfig.Consumption.TotalConsumption;
        memset ((uint8_t *)&RepeaterConfig.Consumption.HourConsumption,0,48); // 22/8/22
    }
    else
    {
        ready_to_update_daily = true;
    }
    if ((hour == (RepeaterConfig.UploadTime.Hour)  %24) &&  (ready_to_send_daily))
    {
        // only once a day at the UploadTime
        // check if repeater config holds date
        if (!   (RepeaterConfig.Consumption.Month > 0) &&
                (RepeaterConfig.Consumption.Month<=12))
        {
            RepeaterConfig.Consumption.Day=(int8_t)day;
            RepeaterConfig.Consumption.Month=(int8_t)month;
            RepeaterConfig.Consumption.Year=(int8_t)year;
        }
        ready_to_send_daily= false;
        modemProcess (); // send daily data to SERVER once a day total consumption is last day total .
    }
    else
    { // to prevent multi send of the daily in the hour of uploadtime y.f. 4/8/2022
        ready_to_send_daily   = true;
    }

#ifdef DEBUG_UART_MODE
    uint32_t tt;
    tt = RepeaterConfig.Consumption.HourConsumption[hour];
    sprintf (str, "hour= %d", tt);
    R_SCI_UART_Write (&Debug_Uart1_ctrl, str, 10);
#endif
}

void modemProcess(void)
{
    ModemSequence (MODEM_START_SEQUENCE);
    while (GsmCompleateStatus ())
    {
        if (Ltutmpindexin)
        {
            Gsm_Handler( Ltutemputartin, Ltutmpindexin , DATA_IN_EVENT);
#ifdef DEBUG_UART_MODE3
//            R_BSP_SoftwareDelay (1000, bsp_delay_units); // delay 1 sec                               {
            R_SCI_UART_Write (&Debug_Uart1_ctrl, Ltutemputartin, Ltutmpindexin);
//            R_BSP_SoftwareDelay (1000, bsp_delay_units); // delay 1 sec                               {

#endif
            R_AGT_Disable(&Pulser_Timer0_Generic_ctrl);
            Ltutmpindexin=0;
            R_AGT_Enable(&Pulser_Timer0_Generic_ctrl);
        }
        else
            if (LtuGSM_Timeout)
            {
                LtuGSM_Timeout = false;
                Gsm_Handler( 0, 0 , TIME_OUT_EVENT);
            }
#ifdef DEBUG_UART_MODE3
        if (Ltutmpindexout)
        {
            R_SCI_UART_Write (&Debug_Uart1_ctrl, Ltutemputartout, Ltutmpindexout);
            R_BSP_SoftwareDelay (500, bsp_delay_units); // delay 1 sec                               {
            R_AGT_Disable(&Pulser_Timer0_Generic_ctrl);
            Ltutmpindexout=0;
            R_AGT_Enable(&Pulser_Timer0_Generic_ctrl);

        }
#endif


    }
    ModemSequence (MODEM_STOP_SEQUENCE);

}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}



#endif

/* Callback function */
void Pulse_int_callback(external_irq_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    fsp_err_t status;
    rtc_time_t CurrentTimeTmp;
    if (p_args->channel == 5) // to be on safe side
    { // debounce process
        status = R_RTC_CalendarTimeGet (&Yos1_RTC_ctrl, &CurrentTimeTmp);
        if (memcmp ((uint8_t*) &CurrentTimeTmp, (uint8_t*) &LastPulseTime, sizeof(rtc_time_t)))
        {
            RepeaterConfig.Consumption.TotalConsumption++;
            TmpHourConsmption++;
            memcpy ((uint8_t*) &LastPulseTime, (uint8_t*) &CurrentTimeTmp, sizeof(rtc_time_t));
        }

    }
}

/* Callback function */
void yos1_rtc_callback(rtc_callback_args_t *p_args)
{
    /* TODO: add your own code here */

    if (p_args->event == RTC_EVENT_ALARM_IRQ)
    {

 //       min_counter++;
        RTC_Event = true;
    }
    /* TODO: add your own code here */

}

void ModemSequence(ModemModeSequence ModeSeq)
{
    R_BSP_PinAccessEnable ();
    /* Write to this pins */
    R_BSP_PinWrite ((bsp_io_port_pin_t) DTR, BSP_IO_LEVEL_HIGH);
    R_BSP_PinWrite ((bsp_io_port_pin_t) LTE_VCC_ENABLE, BSP_IO_LEVEL_HIGH);
    R_BSP_PinWrite ((bsp_io_port_pin_t) WRITE_DISABLE_LTE, BSP_IO_LEVEL_LOW);
    R_BSP_PinWrite ((bsp_io_port_pin_t) WAKE_UP_LTE, BSP_IO_LEVEL_HIGH);

    if (ModeSeq == MODEM_START_SEQUENCE)
    {
        ClrGsmCompleate ();
        R_BSP_PinAccessEnable ();
        /* Write to this pin */
#ifdef EC200N
        // Y.F. 15/12/22
              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_HIGH);
              R_BSP_PinAccessDisable ();
              R_BSP_SoftwareDelay (400, bsp_delay_units); // delay 1 sec
              TimingDelay = 10;


#else
              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
              R_BSP_PinAccessDisable ();
              TimingDelay = 50;

#endif

//        R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);

        R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_RESET_KEY_IN, BSP_IO_LEVEL_HIGH);

#if 0
      HAL_GPIO_WritePin(MODEM_POWER_KEY_PORT, MODEM_POWER_KEY_PIN, GPIO_PIN_RESET );
      HAL_GPIO_WritePin(MODEM_RESET_PORT, MODEM_RESET_PIN, GPIO_PIN_SET );
#endif

        modem_seq_state = 1;
    }

    else

    if (ModeSeq == MODEM_STOP_SEQUENCE)
    {

        R_BSP_PinAccessEnable ();
        /* Write to this pin */
        R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_HIGH);
        R_BSP_PinAccessDisable ();

#if 0
      HAL_GPIO_WritePin(MODEM_POWER_KEY_PORT, MODEM_POWER_KEY_PIN, GPIO_PIN_SET );
#endif

        // HAL_GPIO_WritePin(MODEM_RESET_PORT, MODEM_RESET_PIN, GPIO_PIN_SET );
        TimingDelay = 65;
        modem_seq_state = 5;
    }
    else
        switch (modem_seq_state)
        {
            case 0:
            {

                R_BSP_PinAccessEnable ();
                /* Write to this pin */
#ifdef EC200N
              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
#else
              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
#endif

 //               R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);

                R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_RESET_KEY_IN, BSP_IO_LEVEL_HIGH);
                R_BSP_PinAccessDisable ();

#if 0
          HAL_GPIO_WritePin(MODEM_POWER_KEY_PORT, MODEM_POWER_KEY_PIN, GPIO_PIN_RESET );
          HAL_GPIO_WritePin(MODEM_RESET_PORT, MODEM_RESET_PIN, GPIO_PIN_SET );
#endif

                TimingDelay = 50;
                modem_seq_state++;
                break;

            }
            case 1:
            {
#if 0
        HAL_GPIO_WritePin(MODEM_POWER_KEY_PORT, MODEM_POWER_KEY_PIN, GPIO_PIN_SET );
#endif

                R_BSP_PinAccessEnable ();
                /* Write to this pin */
                R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_HIGH);
                R_BSP_PinAccessDisable ();
                TimingDelay = 3;
                modem_seq_state++;
                break;

            }
            case 2:
            {
#if 0
        HAL_GPIO_WritePin(MODEM_RESET_PORT, MODEM_RESET_PIN, GPIO_PIN_RESET );
#endif
                R_BSP_PinAccessEnable ();
                /* Write to this pin */
                R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_RESET_KEY_IN, BSP_IO_LEVEL_LOW);
                R_BSP_PinAccessDisable ();

                TimingDelay = 47;
                modem_seq_state++;
                break;

            }
            case 3:
            {

                R_BSP_PinAccessEnable ();
                /* Write to this pin */
#ifdef EC200N
              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
#else
              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
#endif

  //              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
                R_BSP_PinAccessDisable ();

#if 0
      HAL_GPIO_WritePin(MODEM_POWER_KEY_PORT, MODEM_POWER_KEY_PIN, GPIO_PIN_RESET );
#endif

                modem_seq_state++;
                Gsm_Init ();
                //  TimingDelay = 100;

                break;

            }
            case 4:
            {
                // modem wake up do nothing wait for stop modem or start modem
            }
            case 5:
            {
                R_BSP_PinAccessEnable ();
                /* Write to this pin */
                R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_HIGH);
                R_BSP_PinAccessDisable ();

#if 0
       HAL_GPIO_WritePin(MODEM_POWER_KEY_PORT, MODEM_POWER_KEY_PIN, GPIO_PIN_SET );
#endif

                TimingDelay = 65;
                modem_seq_state++;
                break;

            }
            case 6:
            {
#if 0
       HAL_GPIO_WritePin(MODEM_POWER_KEY_PORT, MODEM_POWER_KEY_PIN, GPIO_PIN_RESET );
#endif
                R_BSP_PinAccessEnable ();
                /* Write to this pin */
#ifdef EC200N
              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
#else
              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
#endif

  //              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
                R_BSP_PinWrite ((bsp_io_port_pin_t) LTE_VCC_ENABLE, BSP_IO_LEVEL_LOW); // Power off ltu
                R_BSP_PinWrite ((bsp_io_port_pin_t) WAKE_UP_LTE, BSP_IO_LEVEL_LOW);
                R_BSP_PinAccessDisable ();

                modem_seq_state = 0;
                break;
            }
        }
}

void ToggleLeds(void)
{
    if (toggle)
    {
        toggle = false;
        SetLed (PULSER_LED1, false);
        SetLed (PULSER_LED2, true);
    }
    else
    {
        toggle = true;
        SetLed (PULSER_LED1, true);
        SetLed (PULSER_LED2, false);
    }
}
void ClrLeds(void)
{
    R_BSP_PinWrite ((bsp_io_port_pin_t) PULSER_LED1, BSP_IO_LEVEL_HIGH);
    R_BSP_PinWrite ((bsp_io_port_pin_t) PULSER_LED2, BSP_IO_LEVEL_HIGH);
}
void SetLed(ioport_port_pin_t pin_num, bool STATE)
{
    R_BSP_PinAccessEnable ();

    if (pin_num == PULSER_LED1)
    {
        if (STATE)
            /* Write to this pin */
            R_BSP_PinWrite ((bsp_io_port_pin_t) PULSER_LED1, BSP_IO_LEVEL_LOW);

        //     HAL_GPIO_WritePin(LED_PORT, LED_1_PIN,GPIO_PIN_RESET);
        else
            R_BSP_PinWrite ((bsp_io_port_pin_t) PULSER_LED1, BSP_IO_LEVEL_HIGH);

        //      HAL_GPIO_WritePin(LED_PORT, LED_1_PIN,GPIO_PIN_SET);
    }
    else
    {
        if (STATE)
            R_BSP_PinWrite ((bsp_io_port_pin_t) PULSER_LED2, BSP_IO_LEVEL_LOW);

//       HAL_GPIO_WritePin(LED_PORT, LED_2_PIN,GPIO_PIN_RESET);
        else
            R_BSP_PinWrite ((bsp_io_port_pin_t) PULSER_LED2, BSP_IO_LEVEL_HIGH);

//       HAL_GPIO_WritePin(LED_PORT, LED_2_PIN,GPIO_PIN_SET);
    }
    R_BSP_PinAccessDisable ();

}
#if 0
void DelayYos(uint32_t TimeClock)
{
    uint32_t TimingDelaytmp = TimeClock;
    while (TimingDelaytmp--)
        ;

}
#endif

void EnterDefaultConfig(void)
{
    uint32_t *pPulserid = 0x10e00;
    memset ((uint8_t*) &RepeaterConfig, 0, sizeof(RepeaterConfigStruct));
 //   RepeaterConfig.DeviceInitalizaion=0;
    RepeaterConfig.ExtendedAddress = 123456789111;
#ifdef PROGRAME_BY_E2STUDIO
    RepeaterConfig.Moduladdress  = pulser_id;
#else
    //   memcpy (RepeaterConfig.Moduladdress,( unsigned char*) (__IO uint32_t*) FLASH_CF_BLOCK(22) ,4); // ADDRESS 0x10e00 position of pulser id
     RepeaterConfig.Moduladdress = *pPulserid;
#endif

  //  memset ((uint8_t *)&RepeaterConfig.UploadTime,0,sizeof(DailyUploadTimeStruct));
    RepeaterConfig.water_factor=4;// 0.1 m3
    RepeaterConfig.protocol = DLT_PROTOCOL; //DLMS_PROTOCOL;
    RepeaterConfig.MeterSerial.BaudRate = 19200;
    RepeaterConfig.MeterSerial.DataBit = 8;
    RepeaterConfig.MeterSerial.Parity = NONE;
    RepeaterConfig.MeterSerial.StopBit = 1;
    RepeaterConfig.GSMSerial.BaudRate = 115200;
    RepeaterConfig.GSMSerial.DataBit = 8;
    RepeaterConfig.GSMSerial.Parity = NONE;
    RepeaterConfig.GSMSerial.StopBit = 1;
    RepeaterConfig.DebugSerial.BaudRate = 19200;
    RepeaterConfig.DebugSerial.DataBit = 8;
    RepeaterConfig.DebugSerial.Parity = NONE;
    RepeaterConfig.DebugSerial.StopBit = 1;
    memcpy (RepeaterConfig.GsmConfig.APN, "internet", 8);
    // memcpy (RepeaterConfig.GsmConfig.Port ,"2334",4);
    // memcpy (RepeaterConfig.GsmConfig.ServerIP ,"2.55.107.55",11);
    memcpy (RepeaterConfig.GsmConfig.Port, "2334", 4);
//    memcpy (RepeaterConfig.GsmConfig.ServerIP, "31.154.8.241", 12);
    memcpy (RepeaterConfig.GsmConfig.ServerIP, "31.154.79.218", 13);
//  RepeaterConfig.Crc =  CrcBlockCalc((unsigned char*)&RepeaterConfig, sizeof(RepeaterConfigStruct)-2);
    WriteInternalFlashConfig (0, CONFIG_DATA);
}
/* Callback function */
void Battery_low_callback(lvd_callback_args_t *p_args)
{
    /* TODO: add your own code here */
     if ((p_args->current_state)== LVD_CURRENT_STATE_BELOW_THRESHOLD)
         Event_status[0]|= LOW_BATTERY_DETECT;
}

/* Callback function */
void magnet_detect_irq_callback(external_irq_callback_args_t *p_args)
{
    if (p_args->channel == 0) // to be on safe side
    { // debounce process
        MagnetDetect = true; /* TODO: add your own code here */
    }
}
void RTC_Alarm_Set(void)
{
    fsp_err_t status;
    rtc_alarm_time_t hour_alarm;
    hour_alarm.mday_match = false;
    hour_alarm.mon_match = false;
    hour_alarm.year_match = false;
    hour_alarm.mday_match = false;
    hour_alarm.hour_match = false;
    hour_alarm.sec_match = true; // once a hour 8/8/22
    hour_alarm.min_match = true;
    hour_alarm.time.tm_min = 1;
    hour_alarm.time.tm_sec = 10;
      // y.f. every hour set alarm to wake up processsor
    status = R_RTC_CalendarAlarmSet (&Yos1_RTC_ctrl, &hour_alarm);

}

void EnterDefaultTime(void)
{
    fsp_err_t status;

    // check if RTC holds valid time
                     // y.f. 22/8/2022 no need to set default time RTC holds valid time

            static rtc_time_t g_set_time =
            { .tm_hour = 23, .tm_isdst = 0, .tm_mday = 1, .tm_min = 10, .tm_mon = 3, .tm_sec = 12, .tm_wday = 1, .tm_yday = 4,
            .tm_year = 22 };
            // y.f. every hour set alarm to wake up processor
            R_RTC_CalendarTimeSet (&Yos1_RTC_ctrl, &g_set_time);

}

/* Callback function */
void Adc_Comp_callback(adc_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    if ((p_args->channel == ADC_CHANNEL_0) && (p_args->event == ADC_EVENT_SCAN_COMPLETE))
        ready_to_read = true;
}
uint32_t Start_ADC_VBAT(void)
{

    uint16_t temp_vbat1 = 0;
    ready_to_read = false;
    R_ADC_ScanStart (&g_adc0_Vbat_ctrl);
    R_BSP_SoftwareDelay (1000, bsp_delay_units);
    conttemp = 0;
    while (ready_to_read != true) // wait here till adc scan terminated.
        //    conttemp++; // status set while process is in progress
        //  temp_vbat = 0x55;
        R_ADC_Read (&g_adc0_Vbat_ctrl, ADC_CHANNEL_0, &temp_vbat1);
    /** with an analog reference of 3.3 V (VAREF = VDDA) and a 1 V signal input, *****/
    /** the converted result is (1/3.3) × 4095 = 4D9h***/
    return (uint32_t) (temp_vbat1 * 3600) / 4095; // Accuracy  of 3 digits after the decimal point
#ifdef DEBUG_UART_MODE
    char str[10];
    sprintf (str, "Vbat = %d", temp_vbat);
    R_SCI_UART_Write (&Debug_Uart1_ctrl, str, 17);

//            status = R_SCI_UART_Write(&Debug_Uart1_ctrl,str,17);
    R_BSP_SoftwareDelay (100, bsp_delay_units); // delay 1 sec temp to be removed

#endif
 //   return temp_vbat;
}

void ResetConsumptionData(int32_t consumption )
{
    uint8_t bInd;
    memset ( (uint8_t*) &DailyConsumption ,0, sizeof(DailyData)*MAX_LOGGER_DAYS);
    for(bInd=0;bInd<24;bInd++)
    {// clear all hours of day
        RepeaterConfig. Consumption.HourConsumption[bInd]  =0;
    }
    R_ICU_ExternalIrqDisable(&Puls_g_external_irq0_ctrl);
    TmpHourConsmption=0; // clear tmp current hour;
    RepeaterConfig.Consumption.TotalConsumption = consumption;
    RepeaterConfig.freeztotalconsumption = consumption;
    R_ICU_ExternalIrqEnable(&Puls_g_external_irq0_ctrl);
}

void StartWMP_lpm(void)
{
    fsp_err_t status;
            status = R_AGT_Stop (&Pulser_Timer0_Generic_ctrl); // y.f.  stop timer 0 before entering to sleep mode
            Uarts_Close ();
#if 0
            R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);
            setsubclock();
            RTC_Alarm_Set();
            status = R_LPM_Open (&Meter_g_lpm0_ctrl, &Meter_g_lpm0_cfg);
            R_LPM_LowPowerModeEnter(&Meter_g_lpm0_ctrl);
            status = R_LPM_Close (&Meter_g_lpm0_ctrl);
#endif
             //          R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);
              R_BSP_PinAccessEnable ();
              /* Write to this pin */
#ifdef EC200N
              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
#else
              R_BSP_PinWrite ((bsp_io_port_pin_t) MODEM_POWER_KEY_PIN, BSP_IO_LEVEL_LOW);
#endif

              R_BSP_PinWrite ((bsp_io_port_pin_t) LTE_VCC_ENABLE, BSP_IO_LEVEL_LOW); // Power off ltu
              R_BSP_PinWrite ((bsp_io_port_pin_t) WAKE_UP_LTE, BSP_IO_LEVEL_LOW);
              R_BSP_PinWrite ((bsp_io_port_pin_t) DTR, BSP_IO_LEVEL_LOW);


              R_BSP_PinAccessDisable ();

             SetLed (PULSER_LED1, false);
             SetLed (PULSER_LED2, false);
              RTC_Alarm_Set();
              status = R_LPM_Open (&Meter_g_lpm0_ctrl, &Meter_g_lpm0_cfg);
              R_LPM_LowPowerModeEnter(&Meter_g_lpm0_ctrl);
   //           SetLed (PULSER_LED1, true); // to be removed only for test
   //           SetLed (PULSER_LED2, true);

            // wake up from sleep
              Uarts_Initialize ();
#ifdef DEBUG_UART_MODE3
            sprintf (str1, "pulses count = %d", RepeaterConfig.Consumption.TotalConsumption);
            R_SCI_UART_Write (&Debug_Uart1_ctrl, str1, 20);
            R_BSP_SoftwareDelay (100, bsp_delay_units); // delay 1 sec temp to be removed
#endif

}
