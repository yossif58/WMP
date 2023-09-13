/***********************************************************************************************************************
 * File Name    : flash_lp_ep.c
 * Description  : Contains data structures and functions used in flash_lp_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER

 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "hal_data.h"
#include "stdbool.h"
#include "main.h"
#include "flash_lp_ep.h"
#include "crc.h"
#include "utils.h"

/*******************************************************************************************************************//**
 * @addtogroup flash_lp_ep
 * @{
 **********************************************************************************************************************/

/* Flags, set from Callback function */
static volatile _Bool g_b_flash_event_not_blank = false;
static volatile _Bool g_b_flash_event_blank = false;
static volatile _Bool g_b_flash_event_erase_complete = false;
static volatile _Bool g_b_flash_event_write_complete = false;
bool FLashCheck(void);

//fsp_err_t WriteInternalFlashConfig(bool reset_req);

/*
 * private function declarations
 */
//extern long long freeztotalconsumption;
extern uint32_t upgradestate;
extern uint8_t Event_status[];
extern RepeaterConfigStruct RepeaterConfig;
extern DailyData DailyConsumption[]; // count will increase if external flash is avalable
uint8_t read_buff[FLASH_CF_BLOCK_SIZE] =
{ RESET_VALUE };  // read data buffer
uint8_t write_buff[FLASH_CF_BLOCK_SIZE] =
{ RESET_VALUE }; // write data buffer
bool FLashCheck(void)
{
    static int8_t block_num = 1;
    uint16_t crc;
    memcpy ((unsigned char*) &RepeaterConfig, (uint8_t*) FLASH_DF_BLOCK(block_num), sizeof(RepeaterConfigStruct));
    crc = CrcBlockCalc ((unsigned char*) &RepeaterConfig, sizeof(RepeaterConfigStruct) - 2);
    if (RepeaterConfig.Crc != crc)
        return false;
    else if ((RepeaterConfig.Moduladdress == 0) || (RepeaterConfig.Moduladdress == -1))
        return false;
    return true;
}

/*******************************************************************************************************************//**
 * @brief       Opens the Flash_LP module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Upon successful open of Flash_LP module
 * @retval      err   					Upon unsuccessful open
 **********************************************************************************************************************/
fsp_err_t flash_lp_init(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Open Flash_LP module */
    err = R_FLASH_LP_Open (&g_flash_ctrl, &g_flash_cfg);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* Flash_LP Failure message */
        //   APP_ERR_PRINT("** R_FLASH_LP_Open API for Flash_LP failed ** \r\n");
    }

    return err;
}
fsp_err_t ResetUpgradeState(void)
{

    memcpy (&upgradestate, ( unsigned char*) (__IO uint32_t*) FLASH_DF_BLOCK(0) ,4);
    if (upgradestate == UPGRADE_REQ_IS_WAITING)
    {
        upgradestate =0;
        WriteBootStateInFlash(&upgradestate);
        R_SCI_UART_Write (&Debug_Uart1_ctrl, (uint8_t*)"card was upgraded\r\n",20);

    }
}

fsp_err_t WriteBootStateInFlash(uint32_t * UpgradeSate)
{
    fsp_err_t err;
    flash_result_t blankCheck = FLASH_RESULT_BLANK;    // out parameter for BlankCheck()

    /* Reset erase complete flag */
     g_b_flash_event_erase_complete = false;
     /* Erase a block */
     err = R_FLASH_LP_Erase (&g_flash_ctrl, FLASH_DF_BLOCK(0), 1);
     if (FSP_SUCCESS != err)
     {
         //     APP_ERR_PRINT("** R_FLASH_LP_Erase API for Flash_LP failed ** \r\n");
         return err;
     }
     else
     {
         /* wait here for the erase operation to complete (only needed when in BGO) */
         if (g_flash_cfg.data_flash_bgo)
         {
             while (!g_b_flash_event_erase_complete)
             {
                 ;
             }
         }
     }

     /* Perform blank check if erase operation performed */
     if (true == g_b_flash_event_erase_complete)
     {
         /* Reset flag */
         g_b_flash_event_not_blank = false;
         g_b_flash_event_blank = false;
         /* Perform blank check */
         err = R_FLASH_LP_BlankCheck (&g_flash_ctrl, FLASH_DF_BLOCK(0), 4, &blankCheck);
         /* handle error */
         if (FSP_SUCCESS != err)
         {
             /* Flash_LP Failure message */
             //         APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
             return err;
         }
         else
         {
             /* Blank check result */
             if (FLASH_RESULT_BLANK == blankCheck)
             {
                 //          APP_PRINT("Block %d is blank \r\n", block_num);
             }
             else if (FLASH_RESULT_NOT_BLANK == blankCheck)
             {
                 //           APP_PRINT("Block %d is not blank \r\n", block_num);
                 //           APP_PRINT("R_FLASH_LP_Erase() failed to erase %d block\r\n", block_num);
                 return FSP_ERR_ERASE_FAILED;
             }
             else if (FLASH_RESULT_BGO_ACTIVE == blankCheck)
             {
                 //          APP_PRINT("Flash is configured for BGO mode. Result is returned in callback. \r\n");
                 if (FLASH_RESULT_NOT_BLANK == flash_lp_wait_for_blankcheck_flag ())
                 {
                     //             APP_PRINT("Block %d is not blank \r\n", block_num);
                     //              APP_PRINT("R_FLASH_LP_Erase() failed to erase %d block\r\n", block_num);
                     return FSP_ERR_ERASE_FAILED;
                 }
                 else
                 {
                     //              APP_PRINT("Block %d is blank \r\n", block_num);
                 }
             }
             else
             {
                 /* Do nothing */
             }
         }


     /* Data-flash is blank, proceed further */
     /* Read From DATA FLASH */
      /* Reset flag */
     g_b_flash_event_write_complete = false;
     /* Write Data to Data Flash */
     err = R_FLASH_LP_Write (&g_flash_ctrl, (uint32_t) UpgradeSate, FLASH_DF_BLOCK(0), 4);
     if (FSP_SUCCESS != err)
     {
 //      APP_ERR_PRINT("** R_FLASH_LP_Write API for Flash_LP failed ** \r\n");
         return err;
     }
     else
     {
         /* wait here for the write operation to complete (only needed with BGO) */
         if (g_flash_cfg.data_flash_bgo)
         {
             while (!g_b_flash_event_write_complete)
             {
                 ;
             }
         }
         /* RTT output for write success */
         //     APP_PRINT("Write successful \r\n");
     }
     return err;
 }

}

fsp_err_t WriteInternalFlashConfig(bool reset_req, WriteInternal_type dataconfig)
{
    uint32_t ConfigSize;
    uint8_t block_num;
    fsp_err_t err;
    uint8_t *pBuf;
    uint8_t *pTotal;
    flash_result_t blankCheck = FLASH_RESULT_BLANK;    // out parameter for BlankCheck()
    if (dataconfig == CONFIG_DATA)
    {
        block_num = 1;
        RepeaterConfig.Crc = CrcBlockCalc ((unsigned char*) &RepeaterConfig, sizeof(RepeaterConfigStruct) - 2);
        ConfigSize = sizeof(RepeaterConfigStruct);
        pBuf = (uint8_t*) &RepeaterConfig;

    }
    else
    { // daily data in logger
        block_num = 2;
        rtc_time_t CurrentTime;
        uint8_t bInd;
        pBuf = (uint8_t*) &DailyConsumption;
        DailyConsumption[RepeaterConfig.DailyLoggerIndex].PF = ConvertHex2BCD ((uint8_t) RepeaterConfig.water_factor);
        DailyConsumption[RepeaterConfig.DailyLoggerIndex].Day = (int8_t) ConvertHex2BCD (
                (uint8_t) RepeaterConfig.Consumption.Day);
        DailyConsumption[RepeaterConfig.DailyLoggerIndex].Month = (int8_t) ConvertHex2BCD (
                (uint8_t) RepeaterConfig.Consumption.Month);
        DailyConsumption[RepeaterConfig.DailyLoggerIndex].Year = (int8_t) ConvertHex2BCD (
                (uint8_t) RepeaterConfig.Consumption.Year);
        for (bInd = 0; bInd < 24; bInd++)
        { // Convert hex hour consumtion 2 bytes to little endiann BCD
            pTotal = (uint8_t*) &DailyConsumption[RepeaterConfig.DailyLoggerIndex].HourConsumption[bInd];
            uint16_t total1 = RepeaterConfig.Consumption.HourConsumption[bInd];
            if (total1 > 9999)
                total1 = 0;
            for (uint8_t bInd1 = 0; bInd1 < 2; bInd1++)
            { // max value 9999 pulses
                pTotal[bInd1] = (uint8_t) ConvertHex2BCD ((uint8_t) (total1 % 100));
                total1 = total1 / 100;
            }
        }
        uint32_t total = RepeaterConfig.freeztotalconsumption;
        // y.f. 16/8/2022 enter freez   pTotal = (uint8_t*)&DailyConsumption[RepeaterConfig.DailyLoggerIndex].TotalConsumption;
        // uint32_t total= RepeaterConfig.Consumption.TotalConsumption;
        pTotal = (uint8_t*) &DailyConsumption[RepeaterConfig.DailyLoggerIndex].TotalConsumption;
        for (bInd = 0; bInd < 4; bInd++)
        {
            pTotal[bInd] = (uint8_t) ConvertHex2BCD ((uint8_t) total % 100);
            total = total / 100;
        }
        //y.f. 8/8/22 change 2 BCD     DailyConsumption[RepeaterConfig.DailyLoggerIndex].TotalConsumption= RepeaterConfig.Consumption.TotalConsumption;
        DailyConsumption[RepeaterConfig.DailyLoggerIndex].EventRecord[0] = (uint8_t) ConvertHex2BCD (
                (uint8_t) Event_status[0]);
        DailyConsumption[RepeaterConfig.DailyLoggerIndex].EventRecord[1] = (uint8_t) ConvertHex2BCD (
                (uint8_t) Event_status[1]);
        DailyConsumption[RepeaterConfig.DailyLoggerIndex].PrepaymentBalance = 0;
        DailyConsumption[RepeaterConfig.DailyLoggerIndex].Pressure = 0;
        ConfigSize = sizeof(DailyData) * MAX_LOGGER_DAYS;
    }
    while (ConfigSize % 4)
        ConfigSize++; // be sure that we have modulo 4 size

    /* Set write buffer and clear read buffer */
    for (uint16_t index = RESET_VALUE; index < ConfigSize; index++)
    {
        write_buff[index] = pBuf[index];
    }

    /* Perform Blank check before erase */
    /* Reset flag */
    g_b_flash_event_not_blank = false;
    g_b_flash_event_blank = false;
    err = R_FLASH_LP_BlankCheck (&g_flash_ctrl, FLASH_DF_BLOCK(block_num), ConfigSize, &blankCheck);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* Flash_LP Failure message */
        //     APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
        return err;
    }

    else
    {
        /* Blank check result */
        if (FLASH_RESULT_BLANK == blankCheck)
        {
            //   APP_PRINT("Block %d is blank \r\n", block_num);
        }
        else if (FLASH_RESULT_NOT_BLANK == blankCheck)
        {
            //    APP_PRINT("Block %d is not blank \r\n", block_num);
        }
        else if (FLASH_RESULT_BGO_ACTIVE == blankCheck)
        {
            //      APP_PRINT("Flash is configured for BGO mode. Result is returned in callback. \r\n");
            if (FLASH_RESULT_NOT_BLANK == flash_lp_wait_for_blankcheck_flag ())
            {
                //          APP_PRINT("Block %d is not blank \r\n", block_num);
            }
            //     else
            {
                //       APP_PRINT("Block %d is blank \r\n", block_num);
            }
        }
        else
        {
            /* Do nothing */
        }
    }

    /* Reset erase complete flag */
    g_b_flash_event_erase_complete = false;
    /* If not blank, then perform erase operation */
    // if((FLASH_RESULT_NOT_BLANK == blankCheck) || (FLASH_RESULT_NOT_BLANK == flash_lp_wait_for_blankcheck_flag()))
    {
        /* Erase a block */
        err = R_FLASH_LP_Erase (&g_flash_ctrl, FLASH_DF_BLOCK(block_num), NUM_BLOCKS);
        if (FSP_SUCCESS != err)
        {
            //     APP_ERR_PRINT("** R_FLASH_LP_Erase API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            /* wait here for the erase operation to complete (only needed when in BGO) */
            if (g_flash_cfg.data_flash_bgo)
            {
                while (!g_b_flash_event_erase_complete)
                {
                    ;
                }
            }
            /* RTT output for erase success */
            //         APP_PRINT("Erase Successful \r\n");
        }
    }

    /* Perform blank check if erase operation performed */
    if (true == g_b_flash_event_erase_complete)
    {
        /* Reset flag */
        g_b_flash_event_not_blank = false;
        g_b_flash_event_blank = false;
        /* Perform blank check */
        err = R_FLASH_LP_BlankCheck (&g_flash_ctrl, FLASH_DF_BLOCK(block_num), ConfigSize, &blankCheck);
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* Flash_LP Failure message */
            //         APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            /* Blank check result */
            if (FLASH_RESULT_BLANK == blankCheck)
            {
                //          APP_PRINT("Block %d is blank \r\n", block_num);
            }
            else if (FLASH_RESULT_NOT_BLANK == blankCheck)
            {
                //           APP_PRINT("Block %d is not blank \r\n", block_num);
                //           APP_PRINT("R_FLASH_LP_Erase() failed to erase %d block\r\n", block_num);
                return FSP_ERR_ERASE_FAILED;
            }
            else if (FLASH_RESULT_BGO_ACTIVE == blankCheck)
            {
                //          APP_PRINT("Flash is configured for BGO mode. Result is returned in callback. \r\n");
                if (FLASH_RESULT_NOT_BLANK == flash_lp_wait_for_blankcheck_flag ())
                {
                    //             APP_PRINT("Block %d is not blank \r\n", block_num);
                    //              APP_PRINT("R_FLASH_LP_Erase() failed to erase %d block\r\n", block_num);
                    return FSP_ERR_ERASE_FAILED;
                }
                else
                {
                    //              APP_PRINT("Block %d is blank \r\n", block_num);
                }
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    /* Data-flash is blank, proceed further */
    /* Read From DATA FLASH */
    memcpy (read_buff, (uint8_t*) FLASH_DF_BLOCK(block_num), ConfigSize);
    /* Print the read data on RTT viewer */
//   APP_PRINT("Data Read:\r\n");
//  READ_DATA_PRINT(read_buff);
    /* Reset flag */
    g_b_flash_event_write_complete = false;
    /* Write Data to Data Flash */
    err = R_FLASH_LP_Write (&g_flash_ctrl, (uint32_t) write_buff, FLASH_DF_BLOCK(block_num), ConfigSize);
    if (FSP_SUCCESS != err)
    {
//      APP_ERR_PRINT("** R_FLASH_LP_Write API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
        /* wait here for the write operation to complete (only needed with BGO) */
        if (g_flash_cfg.data_flash_bgo)
        {
            while (!g_b_flash_event_write_complete)
            {
                ;
            }
        }
        /* RTT output for write success */
        //     APP_PRINT("Write successful \r\n");
    }

    /* Read From DATA FLASH */
    memcpy (read_buff, (uint8_t*) FLASH_DF_BLOCK(block_num), ConfigSize);
    /* Comparing read_buff and write_buff */
    if ( RESET_VALUE == memcmp (read_buff, write_buff, ConfigSize))
    {

        if (reset_req) // check if reset is required
            NVIC_SystemReset ();

        /* Print the read data on RTT viewer */
//       APP_PRINT("Data Read:\r\n");
//      READ_DATA_PRINT(read_buff);
    }
    else
    {
        /* Print error message on RTT and return error value */
//       APP_PRINT("\r\nRead data does not matches Write data.\r\n");
//       APP_PRINT("\r\nPlease check configuration and addresses...\r\n");
        return FSP_ERR_PARITY;
    }

    return err;
}
fsp_err_t GetPreviousDailyLoggerFromFlash(uint8_t *Datein, uint8_t *pDataOut)
{
// if lastday ind equ 0 = retrieve index should be last day = logger_ind . one day at time
    uint16_t IndexInFlash;
    uint8_t block_num = 2;
    fsp_err_t err = FSP_SUCCESS;    // error status
    DailyData *pDaily;
    pDaily = (DailyData*) pDataOut;
    for (IndexInFlash = 0; IndexInFlash < MAX_LOGGER_DAYS; IndexInFlash++)
    {
        memcpy (pDataOut,
                (unsigned char*) (__IO uint32_t*) FLASH_DF_BLOCK(block_num) + sizeof(DailyData) * IndexInFlash,
                sizeof(DailyData));
        if (memcmp ((char*) &pDaily->Year, (char*) Datein, 3) == 0)
            return err;
    }
    return FSP_ERR_INVALID_ARGUMENT;
}

fsp_err_t GetDailyLoggerFromFlash(uint16_t lastdayind, uint8_t *pDataOut)
{
// if lastday ind equ 0 = retrieve index should be last day = logger_ind . one day at time
    uint16_t IndexInFlash;
    uint8_t block_num = 2;
    fsp_err_t err = FSP_SUCCESS;    // error status
    if (RepeaterConfig.DailyLoggerIndex >= lastdayind)
        IndexInFlash = RepeaterConfig.DailyLoggerIndex - lastdayind;
    else
        IndexInFlash = MAX_LOGGER_DAYS - lastdayind + RepeaterConfig.DailyLoggerIndex;
    memcpy (pDataOut, (unsigned char*) (__IO uint32_t*) FLASH_DF_BLOCK(block_num) + sizeof(DailyData) * IndexInFlash,
            sizeof(DailyData));
    if (pDataOut[0] == 0xff)
        return FSP_ERR_INVALID_ARGUMENT; // data not initilaize yet
    return err;
}

fsp_err_t GetDailyLoggerALLFromFlash(void)
{
    uint8_t block_num = 2;
    fsp_err_t err = FSP_SUCCESS;    // error status
    memcpy ((uint8_t*) &DailyConsumption, (unsigned char*) (__IO uint32_t*) FLASH_DF_BLOCK(block_num),
            sizeof(DailyData) * MAX_LOGGER_DAYS);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function calls APIs to perform read/write/erase and other operations on Code FLASH.
 * @param[IN]   Block number for code flash operations
 * @retval      FSP_SUCCESS             Upon successful operations on Code FLASH
 * @retval      err                     Upon unsuccessful operations on Code FLASH
 **********************************************************************************************************************/
fsp_err_t flash_lp_code_flash_operations(uint8_t block_num, uint8_t *datain, uint16_t datalen)
{

    fsp_err_t err = FSP_SUCCESS;    // error status

    while (datalen % 4)
        datalen++; // be sure that we have modulo 4 size

    /* Clear Flash Access Window */
    err = R_FLASH_LP_AccessWindowClear(&g_flash_ctrl);
    // uint8_t read_buff[FLASH_CF_BLOCK_SIZE] = {RESET_VALUE};  // read data buffer
    // uint8_t write_buff[FLASH_CF_BLOCK_SIZE] = {RESET_VALUE}; // write data buffer
    flash_result_t blankCheck = FLASH_RESULT_BLANK;    // out parameter for BlankCheck()

    /* Perform blank check */
    err = R_FLASH_LP_BlankCheck (&g_flash_ctrl, FLASH_CF_BLOCK(block_num), datalen, &blankCheck);
    /* error handle */
    if (FSP_SUCCESS != err)
    {
//        APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
        /* Print BlankCheck result */
        if (FLASH_RESULT_BLANK == blankCheck)
        {
//          APP_PRINT("Block %d is blank \r\n", block_num);
        }
        else if (FLASH_RESULT_NOT_BLANK == blankCheck)
        {
//          APP_PRINT("Block %d is not blank \r\n", block_num);
        }
        else
        {
            /* Do nothing */
        }
    }

    /* Reset erase complete flag */
    g_b_flash_event_erase_complete = false;
    /* If not blank, then perform erase operation */
    if (FLASH_RESULT_NOT_BLANK == blankCheck)
    {
        /* Erase a block */
        err = R_FLASH_LP_Erase (&g_flash_ctrl, FLASH_CF_BLOCK(block_num), NUM_BLOCKS);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
//          APP_ERR_PRINT("** R_FLASH_LP_Erase API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            //        APP_PRINT("Erase Successful \r\n");
            g_b_flash_event_erase_complete = true;
        }

    }

    /* Perform Blank check after erase operation */
    if (true == g_b_flash_event_erase_complete)
    {
        err = R_FLASH_LP_BlankCheck (&g_flash_ctrl, FLASH_CF_BLOCK(block_num), datalen, &blankCheck);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
            //         APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            /* Print BlankCheck result */
            if (FLASH_RESULT_BLANK == blankCheck)
            {
//              APP_PRINT("Block %d is blank \r\n", block_num);
            }
            else if (FLASH_RESULT_NOT_BLANK == blankCheck)
            {
                //            APP_PRINT("Block %d is not blank \r\n", block_num);
                //             APP_PRINT("R_FLASH_LP_Erase() failed to erase %d block\r\n", block_num);
                return FSP_ERR_ERASE_FAILED;
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    /* If code-flash is blank proceed further */
    /* Read code flash data */
//  memcpy(read_buff, (uint8_t*)FLASH_CF_BLOCK(block_num), FLASH_CF_BLOCK_SIZE);
    /* Print the read data on RTT viewer */
    // APP_PRINT("Data Read:\r\n");
//  READ_DATA_PRINT(read_buff);
    /* Start sector write operation */
    err = R_FLASH_LP_Write (&g_flash_ctrl, (uint32_t) datain, FLASH_CF_BLOCK(block_num), datalen);
    /* error handle */
    if (FSP_SUCCESS != err)
    {
//      APP_ERR_PRINT("** R_FLASH_LP_Write API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
//       APP_PRINT("Write successful\n");
    }

    /* Read code flash data */
    memcpy (read_buff, (uint8_t*) FLASH_CF_BLOCK(block_num), datalen);

    /* Comparing read_buff and write_buff */
    if ( RESET_VALUE == memcmp (read_buff, datain, datalen))
    {
        /* Print the read data on RTT viewer */
//      APP_PRINT("Data Read:\r\n");
//      READ_DATA_PRINT(read_buff);
    }
    else
    {
        /* Print error message on RTT and return error value */
        //     APP_PRINT("\r\nRead data does not matches Write data.\r\n");
//       APP_PRINT("\r\nPlease check configuration and addresses...\r\n");
        return FSP_ERR_PARITY;
    }
    /* Code flash access window example */
//   APP_PRINT("\r\n************************************\r\n");
//    APP_PRINT("*  Code Flash Access Window example\r\n");
//    APP_PRINT("************************************\r\n");
    /* Block number for access window example */
    //  block_num = block_num_input(); y.f. temp 0 to be removed
    //   block_num = 1; // start from 0x401000400
    /* Set Access window.
     * CAUTION: Highly recommended not to use this function if not aware of consequences OR use it with the accessWindowClear API at the end of application.
     * This API locks the Code flash and the lock retains even after power cycle.
     * Which means, even after power cycle, user will not be able to program the code to code flash if the
     * access window is wrongly set.
     *
     * WORKAROUND: If uses uses accessWindowSet and locks the window. Flash can be unlocked by running different application
     * performing just "open" call and "accessWindowClear()" running from the RAM.
     */
    // y.f. removed locked are  err = R_FLASH_LP_AccessWindowSet(&g_flash_ctrl, FLASH_CF_START_ADDR(block_num), FLASH_CF_END_ADDR(block_num));
    /* error handle */
    if (FSP_SUCCESS != err)
    {
        // APP_ERR_PRINT("** R_FLASH_LP_AccessWindowSet API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
        //   APP_PRINT("Code flash Access window set\n");

        /* Erase block again */
        err = R_FLASH_LP_Erase (&g_flash_ctrl, FLASH_CF_BLOCK(block_num), NUM_BLOCKS);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
            //       APP_ERR_PRINT("** R_FLASH_LP_Erase API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            //      APP_PRINT("Erase Successful \r\n");
        }

        /* Perform write operation and check error code */
        err = R_FLASH_LP_Write (&g_flash_ctrl, (uint32_t) datain, FLASH_CF_BLOCK(block_num), datalen);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
//          APP_ERR_PRINT("** R_FLASH_LP_Write API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            //       APP_PRINT("Write successful\n");
        }

        /* Clear Flash Access Window */
        err = R_FLASH_LP_AccessWindowClear (&g_flash_ctrl);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
//          APP_ERR_PRINT("** R_FLASH_LP_AccessWindowClear API for Flash_LP failed ** \r\n");
        }
        else
        {
//          APP_PRINT("Code flash Access window cleared \r\n");
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions checks the flag for blank check.
 * @param[IN]   block_num				Block number of Data-Flash.
 * @retval      FLASH_RESULT_BLANK		If block is blank.
 * @retval		FLASH_RESULT_NOT_BLANK	If block is not blank.
 **********************************************************************************************************************/
flash_result_t flash_lp_wait_for_blankcheck_flag(void)
{
    /* Wait for callback function to set flag */
    while (!(g_b_flash_event_not_blank || g_b_flash_event_blank))
    {
        ;
    }
    if (g_b_flash_event_not_blank)
    {
        /* Reset flag */
        return FLASH_RESULT_NOT_BLANK;
    }
    else
    {
        /* Reset flag */
        return FLASH_RESULT_BLANK;
    }
}

/*******************************************************************************************************************//**
 * @brief       Closes the Flash_LP module.
 * @retval      FSP_SUCCESS             Upon successful close of Flash_LP module.
 * @retval      err						Upon unsuccessful close.
 **********************************************************************************************************************/
fsp_err_t flash_lp_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Close Flash_LP module */
    err = R_FLASH_LP_Close (&g_flash_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* Flash_LP Failure message */
        //      APP_ERR_PRINT("** R_FLASH_LP_Close API for Flash_LP failed ** \r\n");
    }
    return err;
}

/* Callback function for FLASH LP HAL. */
void bgo_callback(flash_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case FLASH_EVENT_NOT_BLANK:
            g_b_flash_event_not_blank = true;
        break;
        case FLASH_EVENT_BLANK:
            g_b_flash_event_blank = true;
        break;
        case FLASH_EVENT_ERASE_COMPLETE:
            g_b_flash_event_erase_complete = true;
        break;
        case FLASH_EVENT_WRITE_COMPLETE:
            g_b_flash_event_write_complete = true;
        break;
        default:
            /* Do nothing */
        break;
    }

}

/*******************************************************************************************************************//**
 * @brief       This function calls APIs to perform read/write/erase and other operations on Code FLASH.
 * @param[IN]   Block number for code flash operations
 * @retval      FSP_SUCCESS             Upon successful operations on Code FLASH
 * @retval      err                     Upon unsuccessful operations on Code FLASH
 **********************************************************************************************************************/
fsp_err_t flash_lp_code_flash_operations_original(uint8_t block_num)
{
    fsp_err_t err = FSP_SUCCESS;    // error status
    uint8_t read_buff1[BLOCK_SIZE] =
    { RESET_VALUE };  // read data buffer
    uint8_t write_buff1[BLOCK_SIZE] =
    { RESET_VALUE }; // write data buffer
    flash_result_t blankCheck = FLASH_RESULT_BLANK;    // out parameter for BlankCheck()
    /* Clear Flash Access Window */
//    err = R_FLASH_LP_AccessWindowClear (&g_flash_ctrl);

    /* Set write buffer and clear read buffer */
    for (uint8_t index = RESET_VALUE; BLOCK_SIZE > index; index++)
    {
        write_buff1[index] = index;
    }

    /* Perform blank check */
    err = R_FLASH_LP_BlankCheck (&g_flash_ctrl, FLASH_CF_BLOCK(block_num), BLOCK_SIZE, &blankCheck);
    /* error handle */
    if (FSP_SUCCESS != err)
    {
//        APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
        /* Print BlankCheck result */
        if (FLASH_RESULT_BLANK == blankCheck)
        {
            //           APP_PRINT("Block %d is blank \r\n", block_num);
        }
        else if (FLASH_RESULT_NOT_BLANK == blankCheck)
        {
//            APP_PRINT("Block %d is not blank \r\n", block_num);
        }
        else
        {
            /* Do nothing */
        }
    }

    /* Reset erase complete flag */
    g_b_flash_event_erase_complete = false;
    /* If not blank, then perform erase operation */
    if (FLASH_RESULT_NOT_BLANK == blankCheck)
    {
        /* Erase a block */
        err = R_FLASH_LP_Erase (&g_flash_ctrl, FLASH_CF_BLOCK(block_num), NUM_BLOCKS);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
//            APP_ERR_PRINT("** R_FLASH_LP_Erase API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
//            APP_PRINT("Erase Successful \r\n");
            g_b_flash_event_erase_complete = true;
        }

    }

    /* Perform Blank check after erase operation */
    if (true == g_b_flash_event_erase_complete)
    {
        err = R_FLASH_LP_BlankCheck (&g_flash_ctrl, FLASH_CF_BLOCK(block_num), BLOCK_SIZE, &blankCheck);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
//            APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            /* Print BlankCheck result */
            if (FLASH_RESULT_BLANK == blankCheck)
            {
                //               APP_PRINT("Block %d is blank \r\n", block_num);
            }
            else if (FLASH_RESULT_NOT_BLANK == blankCheck)
            {
                //               APP_PRINT("Block %d is not blank \r\n", block_num);
                //               APP_PRINT("R_FLASH_LP_Erase() failed to erase %d block\r\n", block_num);
                return FSP_ERR_ERASE_FAILED;
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    /* If code-flash is blank proceed further */
    /* Read code flash data */
    memcpy (read_buff1, (uint8_t*) FLASH_CF_BLOCK(block_num), BLOCK_SIZE);
    /* Print the read data on RTT viewer */
//    APP_PRINT("Data Read:\r\n");
//    READ_DATA_PRINT(read_buff);
    /* Start sector write operation */
    err = R_FLASH_LP_Write (&g_flash_ctrl, (uint32_t) write_buff1, FLASH_CF_BLOCK(block_num), BLOCK_SIZE);
    /* error handle */
    if (FSP_SUCCESS != err)
    {
        //       APP_ERR_PRINT("** R_FLASH_LP_Write API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
//        APP_PRINT("Write successful\n");
    }
    /* Read code flash data */
    memcpy (read_buff1, (uint8_t*) FLASH_CF_BLOCK(block_num), BLOCK_SIZE);

    /* Comparing read_buff and write_buff */
    if ( RESET_VALUE == memcmp (read_buff1, write_buff1, BLOCK_SIZE))
    {
        /* Print the read data on RTT viewer */
//        APP_PRINT("Data Read:\r\n");
//        READ_DATA_PRINT(read_buff);
    }
    else
    {
        /* Print error message on RTT and return error value */
//        APP_PRINT("\r\nRead data does not matches Write data.\r\n");
//        APP_PRINT("\r\nPlease check configuration and addresses...\r\n");
        return FSP_ERR_PARITY;
    }

    /* Code flash access window example */
//    APP_PRINT("\r\n************************************\r\n");
//    APP_PRINT("*  Code Flash Access Window example\r\n");
//    APP_PRINT("************************************\r\n");

    /* Block number for access window example */
  //  block_num = block_num_input();

    /* Set Access window.
     * CAUTION: Highly recommended not to use this function if not aware of consequences OR use it with the accessWindowClear API at the end of application.
     * This API locks the Code flash and the lock retains even after power cycle.
     * Which means, even after power cycle, user will not be able to program the code to code flash if the
     * access window is wrongly set.
     *
     * WORKAROUND: If uses uses accessWindowSet and locks the window. Flash can be unlocked by running different application
     * performing just "open" call and "accessWindowClear()" running from the RAM.
     */
    err = R_FLASH_LP_AccessWindowSet(&g_flash_ctrl, FLASH_CF_START_ADDR(block_num), FLASH_CF_END_ADDR(block_num));
    /* error handle */
    if (FSP_SUCCESS != err)
    {
//        APP_ERR_PRINT("** R_FLASH_LP_AccessWindowSet API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
//        APP_PRINT("Code flash Access window set\n");

        /* Erase block again */
        err = R_FLASH_LP_Erase(&g_flash_ctrl, FLASH_CF_BLOCK(block_num), NUM_BLOCKS );
        /* error handle */
        if (FSP_SUCCESS != err)
        {
 //           APP_ERR_PRINT("** R_FLASH_LP_Erase API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
 //           APP_PRINT("Erase Successful \r\n");
        }

        /* Perform write operation and check error code */
        err = R_FLASH_LP_Write(&g_flash_ctrl, (uint32_t) write_buff1, FLASH_CF_BLOCK(block_num), BLOCK_SIZE);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
 //           APP_ERR_PRINT("** R_FLASH_LP_Write API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
 //           APP_PRINT("Write successful\n");
        }

        /* Clear Flash Access Window */
        err = R_FLASH_LP_AccessWindowClear(&g_flash_ctrl);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
//            APP_ERR_PRINT("** R_FLASH_LP_AccessWindowClear API for Flash_LP failed ** \r\n");
        }
        else
        {
//            APP_PRINT("Code flash Access window cleared \r\n");
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup flash_lp_ep)
 **********************************************************************************************************************/
