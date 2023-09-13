/***********************************************************************************************************************
 * File Name    : flash_lp_ep.h
 * Description  : Contains declarations of data structures and functions used in flash_lp_ep.h.
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

#ifndef FLASH_LP_EP_H_
#define FLASH_LP_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define EP_INFO             	"\r\n\nThis example project demonstrates basic functions of Flash LP driver."\
                                "\r\nAn RTT input from the user triggers Initialization, Erase, Blank Check,"\
                                "\r\nFlash Write and Flash Read operations. User has to choose between the "\
                                "\r\nData Flash or Code Flash operation, and give the input for block number."\
                                "\r\nThe data and status output can be seen on the RTTviewer\r\n"

/* Macros for commands to be received through RTT input */
#define DATA_FLASH_OPERATIONS		1
#define CODE_FLASH_OPERATIONS   	2
#define FLASH_USER_START_ADDR   (0x40100000)

/* Buffer MACROs */
#define BUFF_LEN					(16)
#define ARRAY_INDEX					(0)

/* MACRO for checking if no byte is received */
#define BYTES_RECEIVED_ZERO  		(0)

/* Block size and block number */
#define BLOCK_SIZE					(64)
#define NUM_BLOCKS					(1)

/* Data Flash Macros */
#define FLASH_DF_BLOCK0				(0x40100000)
#if defined(BOARD_RA2E1_EK)
#define FLASH_DF_BLOCK_END			(0x40100FFF)	//FLASH_DF_BLOCK3
#else
#define FLASH_DF_BLOCK_END 			(0x40101C00)	//FLASH_DF_BLOCK7
#endif
#define FLASH_DF_BLOCK_SIZE			((1) * (1024))
#define FLASH_DF_BLOCK(x)			((FLASH_DF_BLOCK0) + (1024) * (uint32_t)(x))

/* Code Flash Macros */
#define FLASH_CF_BLOCK_SIZE			((2) * (1024))
#define FLASH_CF_BLOCK0				(0x00000000)
#define FLASH_CF_BLOCK10			(0x00005000)
#if defined(BOARD_RA2E1_EK)
#define FLASH_CF_BLOCK61			(0x0001EFFF)
#define FLASH_CF_BLOCK_PRE_END		(0x0001F7FF)  	 //FLASH_CF_BLOCK62
#define FLASH_CF_BLOCK_END			(0x0001FFFF)
#elif defined(BOARD_RA4W1_EK)
#define FLASH_CF_BLOCK_253			(0x0007EFFF)
#define FLASH_CF_BLOCK_PRE_END		(0x0007F7FF)	//FLASH_CF_BLOCK254
#define FLASH_CF_BLOCK_END			(0x0007FFFF)
#else
#define FLASH_CF_BLOCK126			(0x0003EFFF)
#define FLASH_CF_BLOCK_PRE_END		(0x0003F7FF)	//FLASH_CF_BLOCK127
#define FLASH_CF_BLOCK_END			(0x0003FFFF)
#endif

#define FLASH_CF_BLOCK(x)			((FLASH_CF_BLOCK0) + (2) * (1024) * (uint32_t)(x))
#define FLASH_CF_START_ADDR(x)		(FLASH_CF_BLOCK(x))
#define FLASH_CF_END_ADDR(x)		((FLASH_CF_BLOCK(x)) + (2) * (FLASH_CF_BLOCK_SIZE))
#define UPGRADE_REQ_IS_WAITING      89
/* This function initialize FLASH LP driver */
fsp_err_t flash_lp_init();

/* This function calls APIs to perform read/write/erase and other operations on Data FLASH */
fsp_err_t flash_lp_data_flash_operations(uint8_t block_num);

/* This function calls APIs to perform read/write/erase and other operations on Code FLASH */
//fsp_err_t flash_lp_code_flash_operations(uint8_t block_num);
fsp_err_t flash_lp_code_flash_operations(uint8_t block_num,uint8_t *datain,uint16_t datalen);
fsp_err_t flash_lp_code_flash_operations_original(uint8_t block_num);

/* This function checks the flag for blank check */
flash_result_t flash_lp_wait_for_blankcheck_flag(void);

/* This function de-initialize FLASH LP driver */
fsp_err_t ResetUpgradeState(void);
fsp_err_t WriteBootStateInFlash(uint32_t * UpgradeSate);
fsp_err_t flash_lp_deinit(void);
fsp_err_t  WriteInternalFlashConfig(bool reset_req,WriteInternal_type dataconfig);
fsp_err_t GetDailyLoggerALLFromFlash(void);
fsp_err_t GetDailyLoggerFromFlash(uint16_t lastdayind,uint8_t *pDataOut);
fsp_err_t GetPreviousDailyLoggerFromFlash(uint8_t *Datein,uint8_t *pDataOut);

#endif /* FLASH_LP_EP_H_ */
