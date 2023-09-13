/**********************************************************************************
*																					*
* FILE NAME:																		*

*   ModeBusProtocol.c																			*
*																					*
* DESCRIPTION:																		*
*   This file contains all routines for the DLT 465 protocol                      *
* HISTORY																			*
*   Ferlandes Yossi																	*
* - CREATION      																	*
*   Yossi Ferlandes 19/11/2009														*
*                                                                                  *
***********************************************************************************/

#include <stdint.h>
#include "utils.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "hal_data.h"
#define DLT_H
#include "DLTProtocol.h"
#include "flash_lp_ep.h"
//#include "FLASH_Program.h"
//#include "meterprotocol.h"


/* local vars defenitions ***********************************************************/
#pragma pack(1)
#ifdef DEBUG_UART_MODE3
void PrintSMSError (char *data);
#endif

uint8_t tmpbyte;
uint16_t KeypadReadDltDI( uint8_t *RxData);
void SendACKRespMsg2Meter(uint8_t Normalack );
uint8_t KeypadCompareDltDI(uint8_t DLTid, uint8_t *RxData);
void SendReadGSMDltRespMsg2Meter(uint8_t *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid);
void SendReadDltRespMsg2Meter(uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid);
bool  ReadConfigFromSMS(uint8_t *pDdataIn, uint16_t Len);
//extern UART_HandleTypeDef UartMETERHandle;
extern long long 	ExtendedAddress;
extern RepeaterConfigStruct RepeaterConfig;
extern void  Error_UartMETERHandler(void);
extern uint32_t SendWrapper(uint8_t *pMsgIn, uint16_t len);
extern void SendData2Meter(uint8_t * DataIn, uint16_t Len);
extern bool write_internal_config_flag;
extern bool write_internal_logger_flag;
unsigned char const DLTPassUser[]     = {0x37,0x44,0x44,0x44,0x33,0x33,0x33,0x33};
unsigned char const DLTID[] = { 0x20    ,1	,1	,1,		
                                0x20	,1	,2	,1,	
                                0x20	,1	,1	,0xf,    
                                0x20	,1	,2	,0xf,    
                                0x20	,1	,0xB	,1,	
                                0x20	,1	,0xb    ,0x10,	
                                0x20	,2	,0	,1,	
                                0x20	,2	,1      ,2,
                                0x20	,2	,1	,3,	
                                0x20	,2	,0x20	,4,	
                                0x20	,3	,0xb     ,1,		//10
                                0x20	,3	,0xb     ,2,		
                                0x20	,3	,0xb     ,3,
                                0x20	,3	,0xb     ,4,
                                0x20	,3	,0xb     ,5,
                                0x20	,3	,0xb     ,6,
                                0x20	,3	,0xb     ,7,
                                0x20	,3	,0xb     ,8,
                                0x20	,3	,0xb     ,9,
                                0x20	,3	,0xb     ,0xa,
                                0x20	,3	,0xb     ,0xb,		//20
                                0x20	,3	,0xb     ,0xc,		
                                0x20	,3	,0xb     ,0xd,
                                0x20	,3	,0xb     ,0xe,
                                0x20	,3	,0xb     ,0xf,
                                0x20	,3	,0xb     ,0x10,
                                0x20	,3	,0xb     ,0x11,
                                0x20	,3	,0xb     ,0x12,
                                0x20	,3	,0xb     ,0x13,
                                0x20	,3	,0xb     ,0x14,
                                0x20	,3	,0xb     ,0x15,		//30
                                //0x20	,3	,0xb     ,0x15,		
								0x00, 0x01, 0xFF, 0x00,		//M.L.03/11/11
                                0x02, 0x80, 0x01, 0x04,
                                0x04, 0x00, 0x04, 0x01,
								0x04, 0x00, 0x01, 0x01,
//                                0x00, 0x90, 0x02, 0x00,
//                                0x04, 0x00, 0x10, 0x09,

								0x02, 0x03, 0x00, 0x00,		//M.L.
//								0x00, 0x0B, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x01,
								0x00, 0x0B, 0x00, 0x01,
								0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00,		//40
								0x00, 0x00, 0x00, 0x00,
								0x04, 0x09, 0x10, 0x01,
								0x04, 0x09, 0x11, 0x01,
								0x02, 0x80, 0x01, 0x02,
								0x00, 0x90, 0x02, 0x00,
								0x02, 0x80, 0x01, 0x06,
								0x04, 0x00, 0x01, 0x02,
								0x02, 0x80, 0x01, 0x03,
								0x04, 0x00, 0x10, 0x09,
								0x02, 0x80, 0x00, 0x02,		//50
								0x3c, 0x23, 0x00, 0x00,
								0x04, 0x05, 0x01, 0x00,
								0x02, 0x80, 0x01, 0x06,
								0x04, 0x80, 0x00, 0x01,
                                                                0x04, 0x00, 0x05, 0x03,       // y.f. 17/02/2014 add prepayment state request 
                                                                0x02, 0x80, 0x01, 0x13,       // y.f. 22/09/2020 add for push read data commmnd
                                                                0x41, 0x43, 0x4b, 0x44,       // y.f 22/09/2020
                                                                0x45, 0x45, 0x45, 0x45,       // y.f. 25/10/2020 neighbour list 
                                                                0x45, 0x45, 0x45, 0x46,       // y.f. 18/11 reset ST flash   DLTSET_ST_RESET
                                                                6,    0,    0,     1  ,
                                                                0x0, 0x2, 0xc0 , 0x34 ,       // write mete id = 67 f3 35 33
                                                                0x11, 0x2, 0xc0 , 0x34 ,       // write mete id = 67 f3 35 44 // it can be 0x33 or 0x44 ignore pssword 
                                                                 0,    2, 0xe3 , 0x10 ,       // write apn =43 16 35 33
                                                                 0,    2, 0xe3 , 0x11,      // write APN USER 44 16 35 33   
                                                                 0,    2, 0xe3 , 0x12,      // write APN PASS 45 16 35 33
                                                                 0,    2, 0xe3 , 0x13,       // write DNS -IP 46 16 35 33 
                                                                 0,    2, 0xe3 , 0x15 ,       // write server= ip  48 16 35 33
                                                                 0,    2, 0xe3 , 0x17 ,       // write server port = 4a 16 35 33
                                                                 0xff, 0xff ,0xe3, 0x10,      // Read APN use 0xff as don't care 43 16  
                                                                 0xff, 0xff ,0xe3, 0x11,      // Read APN USER use 0xff as don't care 43 16  
                                                                 0xff, 0xff ,0xe3, 0x12,      // Read APN PASS 0xff as don't care 43 16  
                                                                 0xff, 0xff ,0xe3, 0x13,      // Read DNS -IP use 0xff as don't care 43 16  
                                                                 0xff, 0xff ,0xe3, 0x15,      // Read ip   use 0xff as don't care 43 16  
                                                                 0xff, 0xff ,0xe3, 0x17,      // Read ip port  use 0xff as don't care 43 16
                                                                 4,    0,    4   ,2           // read/ Write meter pulser id (MAC ADDRESS)
                                                              
};	

/************************************************
  gprs configuration

Write Meter ID:
<DI -- 0xc034>
00: 68 aa aa aa aa aa aa 68 04 0c 67 f3 35 33 33 33 
10: 93 6c 79 44 cb 4c d7 16 
00: 68 60 39 46 11 98 19 68 c4 01 37 6d 16 
 error(0x04)
  Fail!



Read Meter ID
<DI -- 0x04000401>
00: 68 aa aa aa aa aa aa 68 11 04 34 37 33 37 b6 16 
00: 68 60 39 46 11 98 19 68 91 0a 34 37 33 37 93 6c 
10: 79 44 cb 4c b4 16 
  199811463960

Write APN:
<DI -- 0xe310>
00: 68 60 39 46 11 98 19 68 04 24 43 16 35 33 33 33 
10: 9c a1 a7 98 a5 a1 98 a7 33 33 33 33 33 33 33 33 
20: 33 33 33 33 33 33 33 33 33 33 33 33 33 33 23 16 
00: 68 60 39 46 11 98 19 68 84 00 f5 16 
  OK!
Write APN_USER:
<DI -- 0xe311>
00: 68 60 39 46 11 98 19 68 04 15 44 16 35 33 33 33 
10: 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 af 
20: 16 
00: 68 60 39 46 11 98 19 68 84 00 f5 16 
  OK!

Write APN_PASSWORD:
<DI -- 0xe312>
00: 68 60 39 46 11 98 19 68 04 15 45 16 35 33 33 33 
10: 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 b0 
20: 16 
00: 68 60 39 46 11 98 19 68 84 00 f5 16 
  OK!

Write DNS_IP:
<DI -- 0xe313>
00: 68 60 39 46 11 98 19 68 04 15 46 16 35 33 33 33 
10: 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 b1 
20: 16 
00: 68 60 39 46 11 98 19 68 84 00 f5 16 
  OK!

Write SERVER_IP:
<DI -- 0xe315>
00: 68 60 39 46 11 98 19 68 04 15 48 16 35 33 33 33 
10: 65 61 68 68 61 64 63 6a 61 68 68 33 33 33 33 db 
20: 16 
00: 68 60 39 46 11 98 19 68 84 00 f5 16 
  OK!

Write SERVER_PORT:
<DI -- 0xe317>
00: 68 60 39 46 11 98 19 68 04 0b 4a 16 35 33 33 33 
10: 65 66 66 67 33 79 16 
00: 68 60 39 46 11 98 19 68 84 00 f5 16 
  OK!

Read APN:
<DI -- 0xe310>
00: 68 60 39 46 11 98 19 68 01 02 43 16 cd 16 
00: 68 60 39 46 11 98 19 68 81 20 43 16 9c a1 a7 98 
10: a5 a1 98 a7 33 33 33 33 33 33 33 33 33 33 33 33 
20: 33 33 33 33 33 33 33 33 33 33 ce 16 
internet



00: 68 51 26 38 01 00 00 68 01 02 43 16 dc 16 
00: 68 51 26 38 01 00 00 68 81 20 43 16 33 33 9c a1 
10: a7 98 a5 a1 98 a7 33 33 33 33 33 33 33 33 33 33 
20: 33 33 33 33 33 33 33 33 33 33 dd 16 

Read SERVER_IP:
<DI -- 0xe315>
00: 68 60 39 46 11 98 19 68 01 02 48 16 d2 16 
00: 68 60 39 46 11 98 19 68 81 11 48 16 65 61 68 68 
10: 61 64 63 6a 61 68 68 33 33 33 33 86 16 
2.55.107.55

Read SERVER_PORT:
<DI -- 0xe317>
00: 68 60 39 46 11 98 19 68 01 02 4a 16 d4 16 
00: 68 60 39 46 11 98 19 68 81 07 4a 16 65 66 66 67 
10: 33 24 16 
2334

Read APN_USER:
<DI -- 0xe311>
00: 68 60 39 46 11 98 19 68 01 02 44 16 ce 16 
00: 68 60 39 46 11 98 19 68 81 11 44 16 33 33 33 33 
10: 33 33 33 33 33 33 33 33 33 33 33 5a 16 


Read APN_PASSWORD:
<DI -- 0xe312>
00: 68 60 39 46 11 98 19 68 01 02 45 16 cf 16 
00: 68 60 39 46 11 98 19 68 81 11 45 16 33 33 33 33 
10: 33 33 33 33 33 33 33 33 33 33 33 5b 16 


Read DNS_IP:
<DI -- 0xe313>
00: 68 60 39 46 11 98 19 68 01 02 46 16 d0 16 
00: 68 60 39 46 11 98 19 68 81 11 46 16 33 33 33 33 
10: 33 33 33 33 33 33 33 33 33 33 33 5c 16 



****************************************************************/




/*************************************************************************************/
/************************** local function defenitions *******************************/


bool  ReadConfigFromSMS(uint8_t *pDdataIn, uint16_t Len)
{
    char *position;   
    uint16_t bInd=0;
    RepeaterConfigStruct tmpRepeaterConfig;
    position = (strstr((char*)pDdataIn,",2,"));
    if (position==0) return 0;  // not found start of string
    position = position+3;
    // extract server ip Ex 31.22.22.154
    memcpy ((uint8_t*)& tmpRepeaterConfig,(uint8_t*)&RepeaterConfig,sizeof (RepeaterConfigStruct));
    memset ((uint8_t*)& tmpRepeaterConfig.GsmConfig,0,sizeof (GSMConfigStruct));// y.f. 3/1/23
    for (bInd = 0 ;bInd < 17 ; bInd++)
    {
        if ((char)(*position) ==',')
            break; 
        if (((char)(*position) =='.') ||
           ((*position <= 0x39) && (*position >=0x30)))
            tmpRepeaterConfig.GsmConfig.ServerIP[bInd]=*position++;
        else  
        {
            if ((*position==0xa) || (*position==0xd))
            {
                position++;
            }
            else
            {// 14/12/22 ignore cr line feed
                PrintSMSError(position);
                return 0; // not valid char
            }
        }
   } 
  
   // RepeaterConfig.GsmConfig.ServerIP  [bInd]=0;
   position++;

   for (bInd = 0 ;bInd < 6 ; bInd++)
   {
        if ((char)(*position) ==',')
            break; 
        if   ((*position <= 0x39) && (*position >=0x30))
            tmpRepeaterConfig.GsmConfig.Port[bInd]=*position++;
        else
        if ((*position==0xa) || (*position==0xd))
        {
            position++;
        }
        else
        {// 14/12/22 ignore cr line feed
            PrintSMSError(position);
            return 0; // not valid char
        }
   } 
 
 //  tmpRepeaterConfig.GsmConfig.Port[bInd]=0;
     position++;
   for (bInd = 0 ;bInd < 20 ; bInd++) // max apn chars
   {
     if (*position ==',') 
     {
         tmpRepeaterConfig.GsmConfig.APN[bInd] = 0;
         memcpy((uint8_t*)&RepeaterConfig,(uint8_t*)&tmpRepeaterConfig,sizeof(RepeaterConfigStruct)); //4/1/23 only if everything is OK change RepeaterConfig
        return 1;
     }
     tmpRepeaterConfig.GsmConfig.APN[bInd] = *position++;
   }
   PrintSMSError(position);
   return 0; // not valid char

}    
  
  
uint32_t HandleConfigRxFromMETER(uint8_t *pDLTMsg, uint16_t len)
{
    DLTReadStruct *pMsg;
    static uint8_t btmp[255];
    static  uint8_t dataout[255];
    uint16_t bInd;
    uint16_t size,dltdi;
    size=len;
    long long address;
    
    if ( KeypadParseDltRxMsg(pDLTMsg, btmp, &size))
    {
         dltdi = KeypadReadDltDI( btmp);
         switch ( dltdi)
         { 
              
             case DLTREAD_EM_READ_WRITE_MAC_ID:
                 // same id for read and write we have to check control code
                 pMsg = (DLTReadStruct*)btmp;
                 if (pMsg->Header.ReadWriteCmd == DLT_READ_COMMAND)
                 {
                    address =  RepeaterConfig.Moduladdress;
                    for (bInd = 2; bInd < 6 ; bInd++)
                    {   // Convert hex addres to little endiann BCD
                            dataout[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
                            address = address/100;

                    }
                    dataout[0] =0;
                    dataout[1] =0;
                    SendReadDltRespMsg2Meter(dataout,6,DLTREAD_EM_READ_WRITE_MAC_ID);
                    return 1;
                 }

                 else          if (pMsg->Header.ReadWriteCmd == DLT_WRITE_COMMAND)
                 {
                     // y.f. 25  25/12/22 remove only in programming

                     RepeaterConfig.Moduladdress=0;
                     pMsg = (DLTReadStruct*)&btmp[0];
                      for (bInd = 0; bInd < 4 ; bInd++)
                      {
                          RepeaterConfig.Moduladdress  =  RepeaterConfig.Moduladdress *100+ ConvertBCD2Hex(pMsg->Info[8+bInd] -0x33);
                      }
                       SendACKRespMsg2Meter(1);
                      //     WriteInternalFlashConfig(0,CONFIG_DATA); // store the configuration
                           write_internal_config_flag = true; // 14/6/22 temp use flag instead of direct write internal
                      return 1;
                 }
                 break;
         case DLTREAD_EM_SERIAL_NUMBER:
                 // for the moment do nothing no place in flash for more meters
            address =  RepeaterConfig.ExtendedAddress;
            for (bInd = 0; bInd < 6 ; bInd++)
            {   // Convert hex addres to little endiann BCD 
                    dataout[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
                    address = address/100;
                  
            }
            SendReadDltRespMsg2Meter(dataout,6,DLTREAD_EM_SERIAL_NUMBER);
            return 1;
           break;
           case  DLTREAD_EM_WRITE_METER_ID:
             case  DLTREAD_EM_WRITE_METER_ID1:
             // extract meter id 
            RepeaterConfig.ExtendedAddress=0;
            pMsg = (DLTReadStruct*)&btmp[0];
            for (bInd = 0; bInd < 6 ; bInd++)
            { 
                RepeaterConfig.ExtendedAddress  =  RepeaterConfig.ExtendedAddress *100+ ConvertBCD2Hex(pMsg->Info[7-bInd] -0x33);
            }
  //          while (1);
            SendACKRespMsg2Meter(1);
       //     WriteInternalFlashConfig(0,CONFIG_DATA); // store the configuration
            write_internal_config_flag = true; // 14/6/22 temp use flag instead of direct write internal
            return 1;
             break;
           case  DLTREAD_EM_WRITE_APN:
                  pMsg = (DLTReadStruct*)btmp;
                  for (bInd = 0; bInd < sizeof(RepeaterConfig.GsmConfig.APN) ; bInd++)
                  {// ignore first 2 chars??
                    RepeaterConfig.GsmConfig.APN[bInd] = pMsg->Info[bInd+2]-0x33;
                  }
                  SendACKRespMsg2Meter(0);
               return 1;
             break;
           case DLTREAD_EM_WRITE_APN_USER:
                  SendACKRespMsg2Meter(0);
               return 1;
             break;
           case DLTREAD_EM_WRITE_APN_PASS:
                  SendACKRespMsg2Meter(0);
               return 1;
             break;
           case DLTREAD_EM_WRITE_DNS_IP:
                  SendACKRespMsg2Meter(0);
               return 1;
             break;
           case  DLTREAD_EM_WRITE_SERVER_IP:
                  pMsg = (DLTReadStruct*)btmp;
                  for (bInd = 0; bInd < sizeof(RepeaterConfig.GsmConfig.ServerIP) ; bInd++)
                  {
                    RepeaterConfig.GsmConfig.ServerIP[bInd] = pMsg->Info[bInd+2]-0x33;
                  }
                  SendACKRespMsg2Meter(0);
                  //     WriteInternalFlashConfig(0,CONFIG_DATA); // store the configuration
                  write_internal_config_flag = true; // 14/6/22 temp use flag instead of direct write internal

               return 1;
             break;
           case  DLTREAD_EM_WRITE_SERVER_PORT:
              pMsg = (DLTReadStruct*)btmp;
              for (bInd = 0; bInd < sizeof(RepeaterConfig.GsmConfig.Port) ; bInd++)
              {
                RepeaterConfig.GsmConfig.Port[bInd] = pMsg->Info[bInd+2]-0x33;
              }
              SendACKRespMsg2Meter(0);
              //     WriteInternalFlashConfig(0,CONFIG_DATA); // store the configuration
              write_internal_config_flag = true; // 14/6/22 temp use flag instead of direct write internal

               return 1;
             break;
          case DLTREAD_EM_READ_APN:                                  // 3/3//2021 READ APN,                                 // 28/02/2021        
         
             memset (dataout, 0x0,30);
             for (bInd = 0; bInd < sizeof(RepeaterConfig.GsmConfig.APN) ; bInd++)
                dataout[bInd] = RepeaterConfig.GsmConfig.APN[bInd];
         
             SendReadGSMDltRespMsg2Meter(dataout,30,DLTREAD_EM_READ_APN);

             return 1;
            break;
            case DLTREAD_EM_READ_APN_USER: 
                      
             memset (dataout, 0x0,30);
             SendReadGSMDltRespMsg2Meter(dataout,15,DLTREAD_EM_READ_APN_USER);

             return 1;
 
            break;
            case DLTREAD_EM_READ_APN_PASS:                             // 4/3/2021
             memset (dataout, 0x0,30);
             SendReadGSMDltRespMsg2Meter(dataout,15,DLTREAD_EM_READ_APN_PASS);
             return 1;
            break;
            case DLTREAD_EM_READ_DNS_IP:                             // 4/3/2021 
             memset (dataout, 0x0,30);
             SendReadGSMDltRespMsg2Meter(dataout,15,DLTREAD_EM_READ_DNS_IP);
             return 1;

            break;
            
            
            
          case DLTREAD_EM_READ_SERVER_IP:                           // 3/3/2021  READ IP
             memset (dataout, 0x0,30);
             memcpy (dataout ,&RepeaterConfig.GsmConfig.ServerIP,15);
             SendReadGSMDltRespMsg2Meter(dataout,15,DLTREAD_EM_READ_SERVER_IP);
              return 1;
            break;
          case DLTREAD_EM_READ_SERVER_PORT:                         // 3/3/2021  READ PORT
             memset (dataout, 0x0,30);
             for (bInd = 0; bInd < 5; bInd++)
             { 
               dataout[bInd] = RepeaterConfig.GsmConfig.Port[bInd];
             }
             SendReadGSMDltRespMsg2Meter(dataout,5,DLTREAD_EM_READ_SERVER_PORT);
 //            WriteInternalFlashConfig(1,CONFIG_DATA); // store the configuration

             return 1;
            break;
         default:
             break;     
         }
    }
  return 0;
}
/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : uint8_t KeypadCompareDltDI(uint8_t DLTid)   		    */
/* - DESCRIPTION     : this routine compares DI field in receiver buffer with       */
/*                     parameter DLTid                                              */
/* - INPUT           : DLTid                                                        */
/* - OUTPUT          : 0 if equals, 1 if notequal                                   */
/* - CHANGES         :                                                              */               											    
/*                                                                                 */
/************************************************************************************/
uint8_t KeypadCompareDltDI(uint8_t DLTid, uint8_t *RxData)
{
	uint8_t result = 0;
	uint16_t bInd;
	DLTReadStruct *pMsg;
	pMsg = (DLTReadStruct*)RxData;

	for (bInd = 0; bInd < 4 ; bInd++)
	{
	  if(pMsg->Data.DICode[3-bInd] != (uint8_t)(DLTID[DLTid*4+bInd] +0x33)){
		result = 1;
		break;
	  }
	} 
	return result;
}
/***********************************************************************************/
/*                                                                                 */
/* - FUNCTION        : uint8_t KeypadReadDltDI(uint8_t DLTid)   		   */
/* - DESCRIPTION     : this returns  DLTid field in receiver buffer with           */
/*                                                                                 */
/* - INPUT           : BUF IN                                                      */
/* - OUTPUT          : returns  0xffff if dldi not found else DLTid                */
/* - CHANGES         :                                                             */               											    
/*                                                                                 */
/************************************************************************************/
uint16_t KeypadReadDltDI( uint8_t *RxData)
{
	//uint8_t result = 0;
  
	uint8_t Ind,DlTdix;
	DLTReadStruct *pMsg;
	pMsg = (DLTReadStruct*)RxData;
        // search in DLTID array to find match 
        for (DlTdix = 0; DlTdix < DLTREAD_MAX_DLTID ; DlTdix++)
        {
          for (Ind = 0; Ind < 4 ; Ind++)
          {
            tmpbyte = pMsg->Data.DICode[3-Ind]-0x33;
            if(((tmpbyte) != (uint8_t)(DLTID[DlTdix*4+Ind] )) && ((uint8_t)(DLTID[DlTdix*4+Ind] != 0xff)))
            break;   
            if (Ind==3)
              return DlTdix;
          } 
         
        }   return 0xffff; // not found
}


/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : KeypadParseDltRxMsg                                          */
/* - DESCRIPTION     : this routine parse the incoming message recived from Meter   */
/* - INPUT           : pDltPayload. Len of string                                   */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     27/7/2011      						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
uint8_t KeypadParseDltRxMsg(uint8_t   *pDLTMsg, uint8_t   *pDLTDdata, uint16_t *pRxMsgLen)
{
  uint16_t bInd;
  DLTReadStruct *pMsg;
  uint8_t CheckSum  =0;
//  long long aExtendedAddress=0;
  for (bInd = 0 ; bInd < MAX_UART_SIZE; bInd++)
  { // search for STX 
    pMsg = (DLTReadStruct*)(pDLTMsg+bInd);
    if (pMsg->Header.Stx == 0x68) break;
  }
  if (  bInd >=MAX_UART_SIZE ) return 0;  // no stx found 
 // if (  bInd <= 12  )  return 0;  // y.f. a11/08/2020 dd min for dlt message 
//#define SEND_ANY_WAY  

  // calculate C.S.
  for (bInd = 0 ; bInd < pMsg->Header.Len+ sizeof(DLTWriteHeader)  ; bInd++)
    CheckSum = CheckSum + ((unsigned char*) pMsg)[bInd];
  // c.s. error
  if (CheckSum != ((unsigned char *)pMsg)[pMsg->Header.Len+ sizeof(DLTWriteHeader)]) return 0;
  if (((unsigned char *)pMsg)[pMsg->Header.Len+ sizeof(DLTWriteHeader)+1] != 0x16) return 0;  
  // copy the rx data 
  *pRxMsgLen = pMsg->Header.Len+ sizeof(DLTWriteHeader)+2;
  memcpy ((unsigned char *)pDLTDdata, (unsigned char *) pMsg,*pRxMsgLen);
  return (1);
}
/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : SendReadDltRespMsg2Meter                                         */
/*                     sends read message response to  meter                        */
/*                                                                                  */
/* - DESCRIPTION     :                                                              */
/* - INPUT           : pDltPayload. Len of string                                   */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     19/7/2011     						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
void SendReadDltRespMsg2Meter(uint8_t *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid)
{ 
//  long long Maddress=0; 
  uint16_t bInd;
  DLTReadStruct *pMsg;
  uint8_t CheckSum  =0;
  uint8_t TmpTxBuf[MAX_UART_SIZE];
  pMsg = (DLTReadStruct*)&TmpTxBuf;
  pMsg->Header.Stx = 0x68;      // STX
  pMsg->Header.Stx1 = 0x68;      // STX
  pMsg->Header.ReadWriteCmd = DLT_READ_COMMAND_RESPONSE;
  long long address =  RepeaterConfig.ExtendedAddress;
  for (bInd = 0; bInd < 6 ; bInd++)
  {   // Convert hex addres to little endiann BCD 
    pMsg->Header.Add[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
    address = address/100;
  }
   
  
  // enter DI code
  for (bInd = 0; bInd < 4 ; bInd++)
  {
    pMsg->Data.DICode[3-bInd] = DLTID[DLTid*4+bInd];
    pMsg->Data.DICode[3-bInd] = pMsg->Data.DICode[3-bInd] +0x33;
  
  }
 
  pMsg->Header.Len= DLTDdataLen+sizeof(DLTReadDataStruct);
  for (bInd = 0; bInd < DLTDdataLen; bInd++)
  {
     pMsg->Info[bInd] = pDLTDdata[bInd] +0x33;
  }
  for (bInd = 0; bInd < DLTDdataLen+sizeof(DLTWriteHeader)+sizeof(DLTReadDataStruct) ; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
  pMsg->Info[DLTDdataLen] = CheckSum;
  pMsg->Info[DLTDdataLen+1] = 0x16;  // ETX
//  CovertBcd2Add((uint8_t *) &pMsg->Header.Add , &Maddress);
  TmpTxsizeBuf = sizeof(DLTReadDataStruct) + sizeof(DLTWriteHeader)+2+DLTDdataLen;
 // UartMETERHandle.RxState = HAL_UART_STATE_READY;
  SendData2Meter(TmpTxBuf, TmpTxsizeBuf);
     
}
/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : SendReadGSMDltRespMsg2Meter                                         */
/*                     sends read GPRS PARAMETERS response to  meter                        */
/*                                                                                  */
/* - DESCRIPTION     :                                                              */
/* - INPUT           : pDltPayload. Len of string                                   */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     3/3/21    						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
void SendReadGSMDltRespMsg2Meter(uint8_t *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid)
{
 //  long long Maddress=0; 
  uint16_t bInd;
  DLTGPRSStruct *pMsg;
  uint8_t CheckSum  =0;
  uint8_t TmpTxBuf[MAX_UART_SIZE];
  pMsg = (DLTGPRSStruct*)&TmpTxBuf;
  pMsg->Header.Stx = 0x68;      // STX
  pMsg->Header.Stx1 = 0x68;      // STX
  pMsg->Header.ReadWriteCmd = DLT_GPRS_RESPONSE;
  long long address =  RepeaterConfig.ExtendedAddress;
  for (bInd = 0; bInd < 6 ; bInd++)
  {   // Convert hex addres to little endiann BCD 
    pMsg->Header.Add[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
    address = address/100;
  }
   
  
  // enter DI code
  for (bInd = 0; bInd < 2 ; bInd++)
  {
    pMsg->GPRS_ID[1-bInd] = DLTID[DLTid*4+bInd+2];
    pMsg->GPRS_ID[1-bInd] = pMsg->GPRS_ID[1-bInd] +0x33;
  
  }
 
  pMsg->Header.Len= DLTDdataLen+2;
  for (bInd = 0; bInd < DLTDdataLen; bInd++)
  {
     pMsg->Info[bInd] = pDLTDdata[bInd] +0x33;
  }
  for (bInd = 0; bInd < DLTDdataLen+sizeof(DLTWriteHeader)+2 ; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
  pMsg->Info[DLTDdataLen] = CheckSum;
  pMsg->Info[DLTDdataLen+1] = 0x16;  // ETX
//  CovertBcd2Add((uint8_t *) &pMsg->Header.Add , &Maddress);
  TmpTxsizeBuf = DLTDdataLen+sizeof(DLTWriteHeader)+4;
  //UartMETERHandle.RxState = HAL_UART_STATE_READY;
  SendData2Meter(TmpTxBuf, TmpTxsizeBuf);
}
/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : SendACKRespMsg2Meter                                         */
/*                     sends read message response to  meter                        */
/*                                                                                  */
/* - DESCRIPTION     :                                                              */
/* - INPUT           :NONE                                                          */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     03/03/2021     						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
void SendACKRespMsg2Meter(uint8_t Normalack )
{ 
 // long long Maddress=0; 
  uint16_t bInd;
  DLTACKStruct *pMsg;
  uint8_t CheckSum  =0;
  uint8_t TmpTxBuf[MAX_UART_SIZE];
  pMsg = (DLTACKStruct*)&TmpTxBuf;
  pMsg->Stx = 0x68;      // STX
  pMsg->Stx1 = 0x68;      // STX
  if (Normalack)
    pMsg->ReadWriteCmd = DLT_WRITE_ACK_RESPONSE;
  else
    pMsg->ReadWriteCmd = DLT_WRITE_GSM_ACK_RESPONSE;
  long long address =  RepeaterConfig.ExtendedAddress;
  for (bInd = 0; bInd < 6 ; bInd++)
  {   // Convert hex addres to little endiann BCD 
    pMsg->Add[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
    address = address/100;
  }
  pMsg->Info=0; 
  for (bInd = 0; bInd < sizeof(DLTACKStruct) ; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
  TmpTxBuf[sizeof(DLTACKStruct)] = CheckSum;
  TmpTxBuf[sizeof(DLTACKStruct)+1] = 0x16;  // ETX
  TmpTxsizeBuf = sizeof(DLTACKStruct) + 2;
  //UartMETERHandle.RxState = HAL_UART_STATE_READY;
  SendData2Meter(TmpTxBuf, TmpTxsizeBuf);
}


bool CovertBcd2Add(uint8_t *BtmpIn ,   long long  * Maddress)
{

  for (uint16_t bInd =0 ; bInd<6 ; bInd++)
  {
    *Maddress = *Maddress*100;
    *Maddress = *Maddress + ConvertBCD2Hex(BtmpIn[5-bInd]);     //   Convert add from bcd to long long 
  }  
  return true;
}
       
          
#ifdef DEBUG_UART_MODE3
void PrintSMSError (char *data)
{
        char Str[50];

        sprintf (( char*)&Str, "This is The place We failed:" );
        data[70]=0;
        strcat (( char*)&Str,( char*)data);
        R_SCI_UART_Write (&Debug_Uart1_ctrl, Str,80);
        R_BSP_SoftwareDelay (500, 1000); // delay 2 sec
}
#endif
