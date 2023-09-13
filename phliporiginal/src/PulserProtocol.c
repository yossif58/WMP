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
#include "hal_data.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"
#define PULSER_PROTOCOL_H
#include "DLTprotocol.h"
#include "PulserProtocol.h"
//#include "FLASH_Program.h"
#include "hal_data.h"
#include "flash_lp_ep.h"
extern void PrepareFlashProgramming(void);
extern void ResetConsumptionData(int32_t consumption);
extern void SendData2Modem(uint8_t * DataIn, uint16_t Len);
extern uint32_t WaitAfterKC;
extern uint32_t CurrentFirmwareVersion;

//extern void RTC_CalendarConfig(TimeStruct *tm);
uint8_t PP_CompareDI(uint8_t DLTid, uint8_t *RxData);
uint8_t PP_SendMsg2Server(uint8_t * pMsgOut,uint8_t *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid);
void PP_SendLastDayData(void);
uint8_t PP_SendReqInitalization(void);
void PP_SendGetParamsRes(void);
void PP_SendUploadHeaderRes(void);
void PP_SendUploadPacketRes(uint16_t NextPacketInd, uint16_t PacketInd);
uint8_t Pulser_State = PP_DISCONNECT_STATE;
extern uint16_t    TmpHourConsmption;
uint8_t Event_status;
static uint8_t dataout[250];
PulserNewFirmwareStruct NewFirmware;

/************************************************************************************
1.	Message Format

[23-5E-DT-23]-[68]-[AA-AA-AA-AA-AA-AA]-[68]-[MT-MT]-[DL]-[DATA]-[CS]-[16]

23-5E-DT-23	Pulse Coordinator Identification
68		DLT Byte
AA-..-AA	Meter Address/ID
68		DLT Byte
MT-MT		Message Type
DL		Data Length
DATA		Message Data
CS		Checksum
16		DLT Byte


Message Types
Message type is the command code for specific function. Like DI and Function Code.
Type	Description
00-00	Data Response
0A-00	Daily Data
09-00	Alert
01-FF	ACK
02-FF	NACK
***********************************************************************************/
/* local vars defenitions ***********************************************************/
#pragma pack(1)
extern uint32_t upgradestate;
uint32_t PP_ExtractDlt(uint8_t *pMsgIn);
uint8_t PP_CompareDI(uint8_t DLTid, uint8_t *RxData);

extern RepeaterConfigStruct RepeaterConfig;
extern void  Error_UartMETERHandler(void);
extern bool write_internal_config_flag;
extern bool write_internal_logger_flag;
extern void RTC_Alarm_Set(void);
extern fsp_err_t ReadBlockOtap(int argc, uint8_t *bufin);
//extern uint32_t UploadTime;
bool CovertBcd2Add(uint8_t *BtmpIn ,   long long  * Maddress);
unsigned char const pulserID[] = {   0,0,         //Data Response
                                     0xa,0,         //Daily Data
                                     9,0,         //Alert
                                     1,0xff,      //ACK
                                     2,0xff,       //NACK
                                     0xc0,1,       // Set Pulser Id
                                     0xc0,2,       // Set Meter ID
                                     0xc0,3,       // Set Clock
                                     0xc0,4,       // Set Pulser Factor
                                     0xc0,5,       // Set Daily upload time,
                                     0xc0,6,       // Set Consumption Reading
                                     0xc1,0,       // Get Parameters,
                                     0xc2,1,       // Get Last Daily Data,
                                     0xc2,2,       // Get Previous daily Data,
                                     0xca,1,       // Reset Event Record,
                                     0xca,2,       // Set Leakage Event Thrshold,
                                     0xca,3,       // Set Low Battery Event Threshold,
                                     0xcc,1,       // Reset Data,
                                     0xcc,2,       // Restart Pulser,
                                     0xcf,0,       // Upload Firmware,
                                     0xcf,1,        // Apply Firmware,
                                     0x1,1,        // Init Request,
                                     0x1,0xaa      // Init Res,
                                     
};	
const uint8_t PulserClientIdentification[4] = {0x23, 0x5e, 0x12, 0x23};  // start message when   meter -->  server
const uint8_t PulserServerIdentification[2] = {0x23, 0x5e};//{0x23, 0x5e, 0xff, 0x23};  // start message when   server --> meter
void SetCosumptionFromServer(uint8_t *pConsumptionBCD);
void SetTimeFromServer(TimeStruct *pTimeSet);
void SetMeterIdFromServer(uint8_t *pMeterIdBCD);
//void SetPulserIdFromServer(uint8_t *pPulserIdBCD);
void SetFactorFromServer(uint8_t FactorBCD);
void SetUploadTimeFromServer(uint8_t *pUploadTime);
/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : ExtractDlt                                                   */
/* - DESCRIPTION     : this routine is called whenever a message is received from   */
/*                    Gprs/Tcp towards meter.                                       */
/* - INPUT           : Len - size of input data                                     */
/*                     pDataIn - pointer to input data buf                          */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     24.01.2020     						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
uint32_t PP_ExtractDlt(uint8_t *pMsgIn)
{
  uint8_t btmp[255];
  long long bTmpAdd=0;
 // long long totalconsumption = 0;
  uint8_t bInd;
  PulserStaruct *pMsg;
  pMsg = (PulserStaruct*)btmp;  
  PulserConnectionRes  *pConRes; 
  PulserSetMeterId *pMeterId; 
  PulserDataRes *pDataRes;
  PulserSetModuleId *pModuleId;
  PulserGetLastDayDdata *pGetLastDay;
  PulserpreviousDayDdata *PrevDate;
 // PulserSetTime *pTimeDate;
  PulserSetConsumption *pSetConsumption;
  PulserUploadTime  *pUploadTime;
  PulserSetTime *pclockSet;
  PulserSetFactor *pSetFactor;
  PulserResetData *pResetData;
  PulserIntalizaionRes *pInitRes;
  PulserUploadHederStruct  *pUploadHeader;
  PulserUploadPacketStruct *pUploadPacket;

  //PulserParamsRespStruct *pParamsRes;

  //rtc_time_t TimeSet;

  uint16_t size;
    if ( PP_ParseRxMsg(pMsgIn, btmp, &size))
    {
      switch (Pulser_State)
      {
          default:
          case PP_DISCONNECT_STATE:
            // no operation is alowd in this state skip message 
          break;
          case PP_WAIT_FOR_METER_ID_STATE:
              switch  (PP_GetDI(btmp))
              {
                  case ACK_COMMAND:
                      WaitAfterKC = 5000; //wait max time till force disconnect
                      return 1;
                      break;
                  case SET_METER_ID:
                    pMeterId = ( PulserSetMeterId*)pMsg->Info;
                    bTmpAdd =0;
                    for(bInd=0;bInd<6;bInd++)
                    {
                      //EMConfigMacADDR0[bInd] =  pMsg->Header.Add[5-bInd];
                      bTmpAdd = bTmpAdd*100;
                      bTmpAdd = bTmpAdd + ConvertBCD2Hex(pMeterId->MeterId[5-bInd]);     //   Convert add from bcd+33 to long long
                    }
                    if (bTmpAdd != RepeaterConfig.ExtendedAddress)
                    {
                      RepeaterConfig.ExtendedAddress = bTmpAdd;
                      //     WriteInternalFlashConfig(0,CONFIG_DATA); // store the configuration
                      write_internal_config_flag = true; // 14/6/22 temp use flag instead of direct write internal

                    }
                    else
                    {
                        // no action is needed
                    }
                      // if meter id samw as same as ExtendedAddress just send ack response
                    if (pMeterId->KeepConnection == 0)
                    {

                        WaitAfterKC = 30; //wait max time till force disconnect
                   }
                    else
                    {
                        WaitAfterKC = 5000; //wait max time till force disconnect

                    }
                    PP_SendACK();
                    Pulser_State = PP_DAILY_DATA_STATE;

                    return 1;
                   break;
                  default:
                      return 0;
                      break;
              }
              break;
          case PP_CONNECTION_REQ_STATE:
             if (PP_GetDI(btmp)==ACK_COMMAND)
             {
                     pConRes = (PulserConnectionRes*)pMsg->Info;
                     if (pConRes->Err_code == 0x0)   //connection success
                     {
                       // for this stage we are requiring the pendig flag and go direct to sending daily data
                       PP_SendDailyData(0,0,0);
                       WaitAfterKC = 5000; //wait max time till force disconnect
                       Pulser_State = PP_DAILY_DATA_STATE;
                       return 1;
                     }
                     else
                     {
                         if (pConRes->Err_code == METER_ID_NOT_RECOGNIZED)
                         {
                             WaitAfterKC = 5000; //wait max time till force disconnect
                             Pulser_State = PP_WAIT_FOR_METER_ID_STATE;
                             return 1;
                         }
                         return 0; // Has to retru connection req
                     }

             }
             else if (PP_GetDI(btmp)==NACK_COMMAND)
             {
                 pConRes = (PulserConnectionRes*)pMsg->Info;
                 if (pConRes->Err_code == METER_ID_NOT_RECOGNIZED)
                 {
                     WaitAfterKC = 5000; //wait max time till force disconnect
                     Pulser_State = PP_WAIT_FOR_METER_ID_STATE;
                     return 1;
                 }
                 return 0; // Has to retru connection req
             }

        break;
        case PP_WAIT_FOR_INITALIZATION_STATE:
            switch  (PP_GetDI(btmp))
            {
                case NACK_COMMAND:
                    WaitAfterKC = 30; //wait max time till force disconnect
                    return 1;

                    break;
                case INIT_RES:
                    pInitRes = (PulserIntalizaionRes*)pMsg->Info;
                    RepeaterConfig.DeviceInitalizaion=1;
                    RepeaterConfig.Consumption.Day=(int8_t)ConvertBCD2Hex((uint8_t )pInitRes->Time.Date); // 6/9/22 enter first date
                    RepeaterConfig.Consumption.Month=(int8_t)ConvertBCD2Hex((uint8_t )pInitRes->Time.Month);
                    RepeaterConfig.Consumption.Year=(int8_t)ConvertBCD2Hex((uint8_t )pInitRes->Time.Year);
                    SetCosumptionFromServer((uint8_t*)&pInitRes->Consumption);
                    SetTimeFromServer((TimeStruct*)&pInitRes->Time);
                    SetMeterIdFromServer((uint8_t*)&pInitRes->MeterId);
                    SetFactorFromServer(pInitRes->Factor);
                    SetUploadTimeFromServer(&pInitRes->UploadTime);

                    if (pInitRes->PendingFunction == 0x0)   //connection success
                    {
                      WaitAfterKC =   30; //wait max time till force disconnect

                    }
                    else
                    {
                        WaitAfterKC = 5000; //wait max time till force disconnect
                        PP_SendACK();
                        Pulser_State = PP_DAILY_DATA_STATE;
                    }

                     return 1;
                break;
                default:
                    return 0;
                break;


            }

            break;
        case PP_DAILY_DATA_STATE:
          switch  (PP_GetDI(btmp))
          {
              case RESET_DATA:
                         pResetData = (PulserResetData*)pMsg->Info;
                         ResetConsumptionData(0);
                         write_internal_logger_flag = true; // 14/6/22 temp use flag instead of direct write internal
                         if (pResetData->KeepConnection == 0)
                         {
                             WaitAfterKC = 30; //wait max time till force disconnect


                         }
                         else
                         {
                            WaitAfterKC = 5000; //wait max time till force disconnect

                         }
                         PP_SendACK();
                         return 1;
                     break;

            case SET_CLOCK:
                  pclockSet = (PulserSetTime*)pMsg->Info;
                  SetTimeFromServer((TimeStruct*)&pclockSet->Time);
                  if (pclockSet->KeepConnection == 0)
                  {
                      WaitAfterKC = 30; //wait max time till force disconnect
                  }
                  else
                  {
                      WaitAfterKC = 5000; //wait max time till force disconnect
                  }
                  PP_SendACK();
                  return 1;
            break;

            case ACK_COMMAND:
              pDataRes = (PulserDataRes*)pMsg->Info;
              SetTimeFromServer((TimeStruct*)&pDataRes->Time);
               if (pDataRes->MisingDays)
               {
                   PP_SendDailyData(0,pDataRes->MisingDays,0);
                   WaitAfterKC = 5000; //wait max time till force disconnect
                   return 1;

               }
               else
               if (pDataRes->KeepConnection == 0)
               {

                   WaitAfterKC = 30; //wait max time till force disconnect
               }
               else
               {
                   WaitAfterKC = 5000; //wait max time till force disconnect
               }
               PP_SendACK();
               return 1;

            break;
            case RESTART_PULSER:
                pResetData = (PulserResetData*)pMsg->Info;

                if (pResetData->KeepConnection == 0)
                  {

                      WaitAfterKC = 30; //wait max time till force disconnect
                  }
                  else
                  {
                      WaitAfterKC = 5000; //wait max time till force disconnect

                  }
                PP_SendACK();
#if 0
                R_BSP_SoftwareDelay (100, 1000); //wait befor reset

                Reset_Handler();

                R_WDT_Open(&g_wdt0_ctrl, &g_wdt0_cfg);
                for(;;)                                                           /* wait until reset */
                 {
                   __NOP();
                 }
             //   NVIC_SystemReset(); // reset card
#endif

                break;
            case SET_METER_ID:
              pMeterId = ( PulserSetMeterId*)pMsg->Info;
              SetMeterIdFromServer((uint8_t*)&pMeterId->MeterId);
                 // if meter id samw as same as ExtendedAddress just send ack response
              if (pMeterId->KeepConnection == 0)
              {

                  WaitAfterKC = 30; //wait max time till force disconnect
              }
              else
              {
                  WaitAfterKC = 5000; //wait max time till force disconnect
              }
              PP_SendACK();
              return 1;
              
             break;
 
           case SET_PULSER_ID:
              pModuleId = ( PulserSetModuleId*)pMsg->Info;

//              SetPulserIdFromServer((uint8_t*)&pModuleId->ModulId);
                 // if meter id samw as same as ExtendedAddress just send ack response
              if (pModuleId->KeepConnection == 0)
              {

                  WaitAfterKC = 30; //wait max time till force disconnect
              }
              else
              {
                  WaitAfterKC = 5000; //wait max time till force disconnect

              }
              //         PP_SendACK();
                       PP_SendNACK(PULSER_ID_NOT_AUTHORIZED); // Y.F. 25/12/22 NO OPTION TO CHANGE PULSER ID BY REMOTE
                         return 1;
              
             break;
          case  SET_CONSUMPTION_READING:
                pSetConsumption = (PulserSetConsumption*)pMsg->Info;
                SetCosumptionFromServer((uint8_t*)&pSetConsumption->consumption);
                if (pSetConsumption->KeepConnection == 0)
                {

                    WaitAfterKC = 30; //wait max time till force disconnect
                }
                else
                {

                    WaitAfterKC = 5000; //wait max time till force disconnect
                }
                PP_SendACK();
                return 1;
            break;
          case SET_DAILY_UPLOAD_TIME:
              pUploadTime = (PulserUploadTime*)pMsg->Info;
              SetUploadTimeFromServer(pUploadTime);
              if (pUploadTime->KeepConnection == 0)
                {

                  WaitAfterKC = 30; //wait max time till force disconnect
                }
                else
                {

                    WaitAfterKC = 5000; //wait max time till force disconnect
                }
                PP_SendACK();
                return 1;

              break;
          case SET_PULSER_FACTOR:
              pSetFactor = (PulserSetFactor*)pMsg->Info;
              SetFactorFromServer( pSetFactor->factor);
              if (pSetFactor->KeepConnection == 0)
                {

                  WaitAfterKC = 30; //wait max time till force disconnect

                }
                else
                {

                    WaitAfterKC = 5000; //wait max time till force disconnect
                }
                PP_SendACK();
                return 1;
              break;
            case GET_PARAMETERS:

                  pResetData = (PulserResetData*)pMsg->Info;
                  PP_SendGetParamsRes();
                  if (pResetData->KeepConnection == 0)
                  {
                     WaitAfterKC = 30; //wait max time till force disconnect
                  }
                  else
                     WaitAfterKC = 5000; //wait max time till force disconnect

                break;
            case GET_PREVIOUS_DAILY_DATA:
                 PrevDate =(PulserpreviousDayDdata*) pMsg->Info;
                 PP_SendDailyData(1,2,(uint8_t *)&PrevDate->Year);
                 if (PrevDate->KeepConnection == 0)
                     WaitAfterKC = 30; //wait max time till force disconnect
                 else
                     WaitAfterKC = 5000; //wait max time till force disconnect
                return 1;
              break;
            case GET_LAST_DAILY_DATA:
                pGetLastDay =(PulserGetLastDayDdata*) pMsg->Info;
                if (pGetLastDay->KeepConnection == 0)
                    WaitAfterKC = 30; //wait max time till force disconnect
                else
                    WaitAfterKC = 5000; //wait max time till force disconnect
                 PP_SendDailyData(0,1,0);
                 return 1;
              break;
            case UPLOAD_FIRMWARE:
                pUploadHeader = (PulserUploadHederStruct*)pMsg->Info;
#ifdef DEBUG_UART_MODE5
                R_SCI_UART_Write (&Debug_Uart1_ctrl, btmp, size); // temp for testing
                R_BSP_SoftwareDelay (200, 1000);
#endif

                if (pUploadHeader->PacketIndex == 0) // header Staruct
                {
                //     pUploadHeader = (PulserUploadHederStruct*)pMsg->Info;
                     // get new file parameters for later use
                     NewFirmware.NewFirmwareVersion = pUploadHeader->FirmwareVersion;
                     memcpy (NewFirmware .NewTotalFileBytes,pUploadHeader->TotalFileBytes,3);
                     NewFirmware .NewTotalBytesInPacket;
                     memcpy (NewFirmware .NewTotalPackets,pUploadHeader->TotalPackets,3);
                     NewFirmware.NewFileCS = pUploadHeader->FileCS ;
                     PrepareFlashProgramming();
                     NewFirmware.NextPacketIndex=1;
                     NewFirmware.TotalBytesReceived=0;
                     NewFirmware.NewTotalBytesInPacket=pUploadHeader->TotalBytesInPacket;
                     if (pUploadHeader->KeepConnection == 0)
                          WaitAfterKC = 30; //wait max time till force disconnect
                      else
                          WaitAfterKC = 5000; //wait max time till force disconnect
                          PP_SendUploadHeaderRes();
                       return 1;
                }
                else
                { // packet struct
                    pUploadPacket = (PulserUploadPacketStruct*)pMsg->Info;
                    if (pUploadPacket->PacketIndex == NewFirmware.NextPacketIndex)
                    {
                        NewFirmware.NextPacketIndex++;
                        NewFirmware.TotalBytesReceived+=pUploadPacket->BytesInPacket;
                        ReadBlockOtap(pUploadPacket->BytesInPacket, pUploadPacket->data);
                        // check for end of file
                        if (pUploadPacket->PacketIndex+1==(uint16_t)NewFirmware.NewTotalPackets[0]+(uint16_t)(NewFirmware.NewTotalPackets[1]<<8))
                        {
                            PP_SendUploadPacketRes(0xffff,pUploadPacket->PacketIndex);
                            R_BSP_SoftwareDelay (3000, 1000); // delay 3 sec to be shure message was sent
                            upgradestate = UPGRADE_REQ_IS_WAITING;
                            // now is time to set flag indicating that there is new firmware for upgrade and jump to vootloade section
                            WriteBootStateInFlash(&upgradestate);
                            R_BSP_SoftwareDelay (1000, 1000); // delay 3 sec to be shure message was sent
                            RM_MCUBOOT_PORT_BootApp(); // jump to boot sector.
                        }
                         else
                        {
                            PP_SendUploadPacketRes(NewFirmware.NextPacketIndex,pUploadPacket->PacketIndex);
                        }

                    }
                    else    PP_SendUploadPacketRes(NewFirmware.NextPacketIndex,pUploadPacket->PacketIndex);
                    WaitAfterKC = 5000; //wait max time till force disconnect
                    return 1;
                }
                break;
            case APPLY_FIRMWARE:
                break;
            default:
              // unknown command go to disconnect state
              //   Pulser_State = PP_DISCONNECT_STATE;
              //   SetGsmCompleate(); //back to sleep


                 return 0;

            break;
        }
        break;
      }

    }
    else
    {
#ifdef DEBUG_UART_MODE

            R_SCI_UART_Write(&Debug_Uart1_ctrl,"received not proper message from server " ,44);
//            R_BSP_SoftwareDelay(1000, bsp_delay_units); // delay 1 sec temp to be removed

#endif
        return 0;
    }
}


/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : PP_ParseRxMsg                                                */
/* - DESCRIPTION     : this routine parse the incoming message recived from Server  */
/* - INPUT           : pDltPayload. Len of string                                   */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     5/12/2021      						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
uint8_t PP_ParseRxMsg(uint8_t   *pDLTMsg, uint8_t   *pDLTDdata, uint16_t *pRxMsgLen)
{
  uint16_t bInd,add_len=0;
  PulserStaruct *pMsg;
 // long long bTmpAdd  =0;
  uint8_t CheckSum  =0;


  for (bInd = 0 ; bInd < MAX_UART_SIZE; bInd++)
  { // search for identificatio string
    pMsg = (PulserStaruct*)(pDLTMsg+bInd);
    if (memcmp((char*)pMsg,(char*)PulserServerIdentification,sizeof(PulserServerIdentification))==0)
    break;   // Pulser Identification found
  }
  if (  bInd >=MAX_UART_SIZE ) return 0;  // no stx found 
  //if ((pMsg->Header.MessaggeType[0] & 0xc0)== 0xc0)  add_len = 1;
  if ((pMsg->Header.Stx != 0x68) && (pMsg->Header.Stx1 != 0x68) && (pMsg->Info[pMsg->Header.Len +1+add_len] !=0x16)  )  return 0;
  // calculate C.S.
  for (bInd = 0 ; bInd < pMsg->Header.Len+ add_len+sizeof(PulserHeaderStaruct)  ; bInd++)
    CheckSum = CheckSum + ((unsigned char*) pMsg)[bInd];
  // c.s. error
  if (CheckSum != ((unsigned char *)pMsg)[pMsg->Header.Len+ add_len+sizeof(PulserHeaderStaruct)]) return 0;
  // copy the rx data 
  *pRxMsgLen = pMsg->Header.Len+ add_len+ sizeof(PulserHeaderStaruct)+2;
  memcpy ((unsigned char *)pDLTDdata, (unsigned char *) pMsg,*pRxMsgLen);
  return (1);
}
uint8_t PP_SendReqInitalization(void)
{
 // RTC_DateTypeDef TempDate;
  long long address=0;
  static uint8_t btmp[255];
  uint8_t bInd;
  uint8_t CheckSum  =0;
  WaitAfterKC = 5000; //wait max time till force disconnect
  PulserStaruct *pMsg;
  pMsg = (PulserStaruct*)btmp;
  PulserInitReq   *PulserInit;
  PulserInit = (PulserInitReq*)&pMsg->Info;
  memcpy ((char*)pMsg,PulserClientIdentification,sizeof(PulserClientIdentification));
  pMsg->Header.Stx  = 0x68;
  pMsg->Header.Stx1 = 0x68;
  memset((uint8_t*) &pMsg->Header.Add,0xaa,6);
  address =   RepeaterConfig.Moduladdress;
  for (bInd = 0; bInd < 4 ; bInd++)
  {   // Convert hex addres to little endiann BCD
    pMsg->Header.ModelId[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
    address = address/100;
  }

  pMsg->Header.MessaggeType[0]=0;
  pMsg->Header.MessaggeType[1]=1;


  pMsg->Header.Len             = 1; //0x2a;
  PulserInit->Etx             = 0x16;
  PulserInit->version = CurrentFirmwareVersion;
  for (bInd = 0 ; bInd < pMsg->Header.Len+ sizeof(PulserHeaderStaruct)  ; bInd++)
    CheckSum= CheckSum + ((unsigned char*) pMsg)[bInd];
  PulserInit->cs=CheckSum;
  SendData2Modem(btmp,sizeof(PulserHeaderStaruct)+sizeof(PulserInitReq));
 return 1;
}
/*********************************************************************************************/
/*                                                                                           */
/* - FUNCTION        : PP_SendDailyData                                                      */
/*                                                                                           */
/* - DESCRIPTION   :[Description for Daily Data (as [DATA]):                                 */
/*                  [DD-MM-YY]-[PF]-[RR-RR-RR-RR]-                                           */
/*                  [CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC-CC]*/
/*                   -[PR-PR-PR-PR]-[BB-BB-BB-BB]-[ER-ER]                                    */
/*                  DD-MM-YY		Date of daily data                                   */
/*                  PF			Pulse Factor 			                     */
/*                  RR-RR-RR-RR		Consumption Cumulative Reading                       */
/*                  CC-CC-�-CC-CC		24-Hourly Consumption (0-23)                 */
/*                  PR-PR-PR-PR		Pressure Measurement                                 */
/*                  BB-BB-BB-BB		Prepayment Balance                                   */
/*                  ER-ER			Event Record                                 */
/*                                                                                           */
/* - INPUT           :  index in logger                                                                 */
/* - OUTPUT          :  pDltPayload. Len of string                                           */
/* - CHANGES         :                 						             */
/* - CREATION     14/2/22       						             */
/*   Ferlandes Yossi                                                                         */
/*                                                                                           */
/*********************************************************************************************/
uint8_t PP_SendDailyData(uint8_t indexdate,uint16_t IndexDay,uint8_t *pDate)
{ 
 // RTC_DateTypeDef TempDate;
  fsp_err_t err=  FSP_SUCCESS;
  static uint8_t btmp[255];
  uint8_t bInd;
  uint8_t CheckSum  =0;
  WaitAfterKC = 5000; //wait max time till force disconnect
  PulserStaruct *pMsg;
  pMsg = (PulserStaruct*)btmp;  
  PulserDailyData   *PulserDaily;
  PulserDaily = (PulserDailyData*)pMsg->Info;
  memcpy ((char*)pMsg,PulserClientIdentification,sizeof(PulserClientIdentification));
  long long address =  RepeaterConfig.ExtendedAddress;
  pMsg->Header.Stx  = 0x68;
  pMsg->Header.Stx1 = 0x68;
  for (bInd = 0; bInd < 6 ; bInd++)
  {   // Convert hex addres to little endiann BCD 
    pMsg->Header.Add[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
    address = address/100;
  } 
  address =   RepeaterConfig.Moduladdress;
  for (bInd = 0; bInd < 4 ; bInd++)
  {   // Convert hex addres to little endiann BCD
    pMsg->Header.ModelId[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
    address = address/100;
  }
  switch (IndexDay)
  {
      case 0:
          pMsg->Header.MessaggeType[0] = 0xa;
          pMsg->Header.MessaggeType[1] = 0;
      break;
      case 1:
          pMsg->Header.MessaggeType[0] = 0xd2;
          pMsg->Header.MessaggeType[1] = 1;
      break;
      case 2:
      default:
          pMsg->Header.MessaggeType[0] = 0xd2;
          pMsg->Header.MessaggeType[1] = 2;
      break;
  }
  if (IndexDay) IndexDay--;
  pMsg->Header.Len             =   66;//0x2a;
  PulserDaily->Etx             = 0x16;
  if (indexdate)
  {
     err = GetPreviousDailyLoggerFromFlash(pDate,(uint8_t*)&PulserDaily->Consumption);
     if   (err  !=FSP_SUCCESS)
     {
         // set null values
         memset((uint8_t*)&PulserDaily->Consumption,0,sizeof(DailyData));
    }
  }
  else
  {
 //    if (IndexDay ==0) // current day take it from the RAM
 //        memcpy((uint8_t*)&PulserDaily->Consumption,(uint8_t*)&RepeaterConfig.Consumption,sizeof(DailyData));
 //    else
         err = GetDailyLoggerFromFlash(IndexDay,(uint8_t*)&PulserDaily->Consumption);
     if   (err  !=FSP_SUCCESS)
     {
        // set null values
        memset((uint8_t*)&PulserDaily->Consumption,0,sizeof(DailyData));
     }
  }
  for (bInd = 0 ; bInd < pMsg->Header.Len+ sizeof(PulserHeaderStaruct)  ; bInd++)
    CheckSum= CheckSum + ((unsigned char*) pMsg)[bInd];
  PulserDaily->cs=CheckSum;
  SendData2Modem(btmp,sizeof(PulserHeaderStaruct)+sizeof(PulserDailyData));  
 return FSP_SUCCESS;
}

/******************************************************************************************************/
/*                                                                                                    */
/* - FUNCTION        : PP_SendACK                                                                     */
/*                                                                                                    */
/* - DESCRIPTION   :[Description for ACK :                                                            */
/*                  [23]-[5E]-[12]-[23]-[68]-[AA-AA-AA-AA-AA-AA]-[68]-[01-FF]-[02]-[EC]-[PF]-[CS]-[16]*/
/*              	Where:                                                                        */                                                          
/*               23-5E-FF-23	Pulse Coordinator Server Response                                     */
/*               01-FF		Message Type (ACK)                                                    */
/*               EC		Error Code                                                            */
/*               PF		Pending Function                                                      */
/* - INPUT           :  none                                                                          */
/* - OUTPUT          :  pDltPayload. Len of string                                                    */
/* - CHANGES         :                 					                	      */
/* - CREATION     15/5/22       						                      */
/*   Ferlandes Yossi                                                                                  */
/*                                                                                                    */
/******************************************************************************************************/
uint8_t PP_SendACK(void)
{ 
  static uint8_t btmp[255];
  uint8_t bInd;
  uint8_t CheckSum  =0;
  PulserStaruct *pMsg;
  pMsg = (PulserStaruct*)btmp;  
  PulserACK *PACKRes;
  PACKRes = (PulserACK*)pMsg->Info;
  memcpy ((char*)pMsg,PulserClientIdentification,sizeof(PulserClientIdentification));
  long long address =  RepeaterConfig.ExtendedAddress;
  pMsg->Header.Stx  = 0x68;
  pMsg->Header.Stx1 = 0x68;
  pMsg->Header.MessaggeType[0] = 1;
  pMsg->Header.MessaggeType[1] = 0xff;
  pMsg->Header.Len             = 0x2;
  PACKRes->Etx                 = 0x16;
  PACKRes->PF= 0;
  PACKRes->EC= 0;
   address =  RepeaterConfig.ExtendedAddress;
   for (bInd = 0; bInd < 6 ; bInd++)
   {   // Convert hex addres to little endiann BCD
     pMsg->Header.Add[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
     address = address/100;
   }
   address =   RepeaterConfig.Moduladdress;
   for (bInd = 0; bInd < 4 ; bInd++)
   {   // Convert hex addres to little endiann BCD
     pMsg->Header.ModelId[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
     address = address/100;
   }
  for (bInd = 0 ; bInd < pMsg->Header.Len+ sizeof(PulserHeaderStaruct)  ; bInd++)
    CheckSum= CheckSum + ((unsigned char*) pMsg)[bInd];
  PACKRes->CS=CheckSum;
  SendData2Modem(btmp,sizeof(PulserHeaderStaruct)+sizeof(PulserACK));
  return 1; 
}

uint8_t PP_SendNACK(uint8_t error)
{
  static uint8_t btmp[255];
  uint8_t bInd;
  uint8_t CheckSum  =0;
  PulserStaruct *pMsg;
  pMsg = (PulserStaruct*)btmp;
  PulserACK *PACKRes;
  PACKRes = (PulserACK*)pMsg->Info;
  memcpy ((char*)pMsg,PulserClientIdentification,sizeof(PulserClientIdentification));
  long long address =  RepeaterConfig.ExtendedAddress;
  pMsg->Header.Stx  = 0x68;
  pMsg->Header.Stx1 = 0x68;
  pMsg->Header.MessaggeType[0] = 2;
  pMsg->Header.MessaggeType[1] = 0xff;
  pMsg->Header.Len             = 0x2;
  PACKRes->Etx                 = 0x16;
  PACKRes->PF= 0;
  PACKRes->EC= error;
   address =  RepeaterConfig.ExtendedAddress;
   for (bInd = 0; bInd < 6 ; bInd++)
   {   // Convert hex addres to little endiann BCD
     pMsg->Header.Add[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
     address = address/100;
   }
   address =   RepeaterConfig.Moduladdress;
   for (bInd = 0; bInd < 4 ; bInd++)
   {   // Convert hex addres to little endiann BCD
     pMsg->Header.ModelId[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
     address = address/100;
   }
  for (bInd = 0 ; bInd < pMsg->Header.Len+ sizeof(PulserHeaderStaruct)  ; bInd++)
    CheckSum= CheckSum + ((unsigned char*) pMsg)[bInd];
  PACKRes->CS=CheckSum;
  SendData2Modem(btmp,sizeof(PulserHeaderStaruct)+sizeof(PulserACK));
  return 1;
}


/*************************************************************************************/
/*                                                                                   */
/* - FUNCTION        : PP_ReqConnectionFromServer                                    */
/*                                                                                   */
/* - DESCRIPTION   :[23]-[5E]-[DT]-[23]-[YY-NN-NN]-[FW]-[AA-AA-AA-AA-AA-AA]-[CS]-[16]*/
/*   23		Pulser Protocol Byte                                                */
/*   5E		Pulser Classification                                               */
/*   DT		Pulser Device Type                                                  */
/*   23		Pulser Protocol Byte                                                */
/*   YY-NN-NN	Pulser ID (Year + SN)                                               */
/*   FW		Pulser Firmware Version                                             */
/*   AA-..-AA	Meter ID/Address                                                    */
/*   CS		Checksum                                                            */
/* - INPUT           :  none                                                        */
/* - OUTPUT          :  pDltPayload. Len of string                                  */
/* - CHANGES         :                 						    */
/* - CREATION     5/12/21         						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
void PP_ReqConnectionFromServer(void)
{ 
   if (RepeaterConfig.DeviceInitalizaion==0)
   {
        PP_SendReqInitalization();
        Pulser_State = PP_WAIT_FOR_INITALIZATION_STATE;
        return ;
  }
  uint8_t bInd;
  uint8_t CheckSum  =0;
  PulserReqConnection *pMsg;
  uint8_t DLTDdata[250]; 
  pMsg = (PulserReqConnection*)DLTDdata;
  Pulser_State = PP_CONNECTION_REQ_STATE;
  WaitAfterKC = 5000; //wait max time till force disconnect
  memcpy ((char*)pMsg,PulserClientIdentification,sizeof(PulserClientIdentification));
  pMsg->header.Stx= 0x68;
  pMsg->header.Stx1= 0x68;
  pMsg->header.MessaggeType[0]=0;
  pMsg->header.MessaggeType[1]=1;
  pMsg->header.Len = 1;
  pMsg->version = CurrentFirmwareVersion;
  long long address =  RepeaterConfig.ExtendedAddress;
  for (bInd = 0; bInd < 6 ; bInd++)
  {   // Convert hex addres to little endiann BCD 
    pMsg->header.Add[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
    address = address/100;
  }
  address =   RepeaterConfig.Moduladdress;
  for (bInd = 0; bInd < 4 ; bInd++)
  {   // Convert hex addres to little endiann BCD 
    pMsg->header.ModelId[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
    address = address/100;
  }
  for (bInd = 0; bInd < sizeof(PulserReqConnection)-2; bInd++)
    CheckSum = CheckSum + DLTDdata[bInd];
  pMsg->cs = CheckSum;
  pMsg->Etx = 0x16;      // etx   
  SendData2Modem(DLTDdata,sizeof(PulserReqConnection));  
}



void PP_SendGetParamsRes(void)
{
    PulserParamsRespStruct *pMsg;
    //    rtc_time_t CurrentTime;
    rtc_time_t  CurrentTime;
    uint8_t bInd;
    uint8_t CheckSum  =0;
    pMsg = (PulserParamsRespStruct*)dataout;
    WaitAfterKC = 5000; //wait max time till force disconnect
    memcpy ((char*)pMsg,PulserClientIdentification,sizeof(PulserClientIdentification));
    pMsg->header.Stx= 0x68;
    pMsg->header.Stx1= 0x68;
    pMsg->header.MessaggeType[0]=0xd1;
    pMsg->header.MessaggeType[1]=0;
    pMsg->header.Len = 31;
    pMsg->Version = CurrentFirmwareVersion;
    long long address =  RepeaterConfig.ExtendedAddress;
    for (bInd = 0; bInd < 6 ; bInd++)
    {   // Convert hex addres to little endiann BCD
      pMsg->MeterId[bInd]=pMsg->header.Add[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
      address = address/100;
    }
    address =   RepeaterConfig.Moduladdress;

    for (bInd = 0; bInd < 4 ; bInd++)
    {   // Convert hex addres to little endiann BCD
      pMsg->PulserId[bInd]=pMsg->header.ModelId[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
      address = address/100;
    }
    R_RTC_CalendarTimeGet (&Yos1_RTC_ctrl, &CurrentTime);
    pMsg->Clock[0] = ConvertHex2BCD((uint8_t)CurrentTime.tm_year);
    pMsg->Clock[1] = ConvertHex2BCD((uint8_t)CurrentTime.tm_mon);
    pMsg->Clock[2] = ConvertHex2BCD ((uint8_t)CurrentTime.tm_mday);
    pMsg->Clock[3] = ConvertHex2BCD((uint8_t)CurrentTime.tm_hour);
    pMsg->Clock[4] = ConvertHex2BCD((uint8_t) CurrentTime.tm_min);
    pMsg->Clock[5] = ConvertHex2BCD((uint8_t)CurrentTime.tm_sec);
    uint32_t total= RepeaterConfig.Consumption.TotalConsumption;
    for( bInd = 0; bInd < 4 ; bInd++)
    {
            pMsg->TotalConsumption[bInd]= (uint8_t) ConvertHex2BCD((uint8_t)(total  % 100));
            total = total/100;
    }
    uint8_t *pUpload = (uint8_t* )&RepeaterConfig.UploadTime;
    for (bInd = 0; bInd < 3 ; bInd++)
        pMsg->UpluadTime[bInd]=   ConvertHex2BCD((uint8_t)pUpload[bInd]);
    memset ((uint8_t*)pMsg->LeakageEvent,0,3);
    pMsg->LowBattery[0] =7;
    pMsg->LowBattery[1] =2;
    pMsg->LowBattery[2] =0;
    pMsg->Factor = ConvertHex2BCD((uint8_t)RepeaterConfig.water_factor); // y.f. 7/9/22
     for (bInd = 0; bInd < sizeof(PulserParamsRespStruct)-2; bInd++)
      CheckSum = CheckSum + dataout[bInd];
    pMsg->cs = CheckSum;
    pMsg->Etx = 0x16;      // etx
    SendData2Modem(dataout,sizeof(PulserParamsRespStruct));



}
void PP_SendUploadPacketRes(uint16_t NextPacketInd, uint16_t PacketInd)
{

    PulserUploadPcketResStruct *pMsg;
    uint8_t bInd;
    uint8_t CheckSum  =0;
    pMsg = (PulserUploadPcketResStruct*)dataout;
    WaitAfterKC = 5000; //wait max time till force disconnect
    memcpy ((char*)pMsg,PulserClientIdentification,sizeof(PulserClientIdentification));
    pMsg->header.Stx= 0x68;
    pMsg->header.Stx1= 0x68;
    pMsg->header.MessaggeType[0]=0xdf;
    pMsg->header.MessaggeType[1]=0;
    pMsg->header.Len = 9;
    pMsg->ErrCode=0;
    pMsg->AckNack=1;
    pMsg->PacketIndex=PacketInd;
    pMsg->NextPacketIndex=NextPacketInd;
    pMsg->TotalBytesReceived[2] = NewFirmware.TotalBytesReceived>>16;
    pMsg->TotalBytesReceived[1] = NewFirmware.TotalBytesReceived>>8;
    pMsg->TotalBytesReceived[0] = NewFirmware.TotalBytesReceived & 0xff;

    long long address =  RepeaterConfig.ExtendedAddress;
    for (bInd = 0; bInd < 6 ; bInd++)
    {   // Convert hex addres to little endiann BCD
      pMsg->header.Add[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
      address = address/100;
    }
    address =   RepeaterConfig.Moduladdress;
    for (bInd = 0; bInd < 4 ; bInd++)
    {   // Convert hex addres to little endiann BCD
      pMsg->header.ModelId[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
      address = address/100;
    }
     for (bInd = 0; bInd < sizeof(PulserUploadPcketResStruct)-2; bInd++)
      CheckSum = CheckSum + dataout[bInd];
    pMsg->cs = CheckSum;
    pMsg->Etx = 0x16;      // etx
    SendData2Modem(dataout,sizeof(PulserUploadPcketResStruct));
}



void PP_SendUploadHeaderRes(void)
{
    PulserUploadHederResStruct *pMsg;
    uint8_t bInd;
    uint8_t CheckSum  =0;
    pMsg = (PulserUploadHederResStruct*)dataout;
    WaitAfterKC = 5000; //wait max time till force disconnect
    memcpy ((char*)pMsg,PulserClientIdentification,sizeof(PulserClientIdentification));
    pMsg->header.Stx= 0x68;
    pMsg->header.Stx1= 0x68;
    pMsg->header.MessaggeType[0]=0xdf;
    pMsg->header.MessaggeType[1]=0;
    pMsg->header.Len = 7;
    pMsg->FirmwareVersion = CurrentFirmwareVersion;
    pMsg->ErrCode=0;
    pMsg->AccetUpload=1;
    pMsg->PacketIndex=0;
    pMsg->NextPacketIndex=1;
    long long address =  RepeaterConfig.ExtendedAddress;
    for (bInd = 0; bInd < 6 ; bInd++)
    {   // Convert hex addres to little endiann BCD
      pMsg->header.Add[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
      address = address/100;
    }
    address =   RepeaterConfig.Moduladdress;

    for (bInd = 0; bInd < 4 ; bInd++)
    {   // Convert hex addres to little endiann BCD
      pMsg->header.ModelId[bInd] = ConvertHex2BCD((uint8_t)(address % 100));
      address = address/100;
    }
     for (bInd = 0; bInd < sizeof(PulserUploadHederResStruct)-2; bInd++)
      CheckSum = CheckSum + dataout[bInd];
    pMsg->cs = CheckSum;
    pMsg->Etx = 0x16;      // etx
    SendData2Modem(dataout,sizeof(PulserUploadHederResStruct));
}

/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : uint8_t PP_GetDI(uint8_t DLTid)   	         	    */
/* - DESCRIPTION     : this routine gets  DI field from receiver buffer             */
/* - INPUT           : rx data                                                      */
/* - OUTPUT          : DLTid                                                        */
/* - CHANGES         :                                                              */               											    
/* - CREATION     15/05/22         						    */
/*   Ferlandes Yossi                                                                */                                                                                  
/************************************************************************************/
uint8_t PP_GetDI(uint8_t *RxData)
{
	uint8_t bInd;
	PulserStaruct *pMsg;
	pMsg = (PulserStaruct*)RxData;
	for (bInd = 0; bInd < MAX_CODE_ID ; bInd++)
	{
 	  if   ((pMsg->Header.MessaggeType[0]   == (uint8_t)(pulserID[bInd*2] )) &&
                (pMsg->Header.MessaggeType[1] == (uint8_t)(pulserID[bInd*2+1] )))
		return bInd;
	} 
	return 0xff; // There is no match between msg input for known code
}


void SetCosumptionFromServer(uint8_t *pConsumptionBCD)
{
    uint8_t bInd;
    long long totalconsumption=0;
    for(bInd=0;bInd<4;bInd++)
     {
       totalconsumption = totalconsumption*100;
       totalconsumption = totalconsumption + ConvertBCD2Hex(pConsumptionBCD[3-bInd]);     //   Convert add from bcd+0x33 to long long
     }
    ResetConsumptionData(totalconsumption);
    write_internal_logger_flag = true; // 14/6/22 temp use flag instead of direct write internal

}
void SetTimeFromServer(TimeStruct *pTimeSet)
{

 // ceck validity of time
    rtc_time_t TmpTime;
    if ((pTimeSet->Year >=        0x22) &&
            (pTimeSet->Date <=    0x31) &&
            (pTimeSet->Month <=   0x12) &&
            (pTimeSet->Hours <=   0x24) &&
            (pTimeSet->Minutes <= 0x60) &&
            (pTimeSet->Seconds <= 0x60) )
    {
//y.f. 22/08/22 I have to convert to decimal because timeset routine expect input is in decimal
        TmpTime.tm_year=  ConvertBCD2Hex(pTimeSet->Year);
        TmpTime.tm_mday=ConvertBCD2Hex(pTimeSet->Date);
        TmpTime.tm_mon=ConvertBCD2Hex(pTimeSet->Month);
        TmpTime.tm_hour=ConvertBCD2Hex(pTimeSet->Hours);
        TmpTime.tm_min=ConvertBCD2Hex(pTimeSet->Minutes);
        TmpTime.tm_sec= ConvertBCD2Hex(pTimeSet->Seconds);
        R_RTC_Close(&Yos1_RTC_ctrl);
        R_RTC_Open(&Yos1_RTC_ctrl, &Yos1_RTC_cfg);
        R_RTC_CalendarTimeSet(&Yos1_RTC_ctrl,&TmpTime);
        RTC_Alarm_Set();
    }




}
void SetMeterIdFromServer(uint8_t *pMeterIdBCD)
{
    long long bTmpAdd =0;
    for(uint8_t bInd=0;bInd<6;bInd++)
    {

      bTmpAdd = bTmpAdd*100;
      bTmpAdd = bTmpAdd + ConvertBCD2Hex(pMeterIdBCD[5-bInd]);     //   Convert add from bcd+33 to long long
    }
    if (bTmpAdd != RepeaterConfig.ExtendedAddress)
    {
      RepeaterConfig.ExtendedAddress = bTmpAdd;
      //     WriteInternalFlashConfig(0,CONFIG_DATA); // store the configuration
      write_internal_config_flag = true; // 14/6/22 temp use flag instead of direct write internal

    }

}
#if 0
void SetPulserIdFromServer(uint8_t *pPulserIdBCD)
{
    long long bTmpAdd =0;
    for(uint8_t bInd=0;bInd<4;bInd++)
    {
      bTmpAdd = bTmpAdd*100;
      bTmpAdd = bTmpAdd + ConvertBCD2Hex(pPulserIdBCD[3-bInd]);     //   Convert add from bcd+0x33 to long long
    }
    if (bTmpAdd != RepeaterConfig.Moduladdress)
    {
      RepeaterConfig.Moduladdress = bTmpAdd;
      //     WriteInternalFlashConfig(0,CONFIG_DATA); // store the configuration
      write_internal_config_flag = true; // 14/6/22 temp use flag instead of direct write internal
    }
}
#endif

void SetFactorFromServer(uint8_t FactorBCD)
{
    RepeaterConfig.water_factor =  ConvertBCD2Hex(FactorBCD);
    write_internal_config_flag = true; // 14/6/22 temp use flag instead of direct write internal
}
void SetUploadTimeFromServer(uint8_t *pUploadTime)
{
    uint8_t *pMsg = (uint8_t*)&RepeaterConfig.UploadTime;
    for(uint8_t bInd=0;bInd<3;bInd++)
    {
        pMsg[bInd] =ConvertBCD2Hex(pUploadTime[bInd]);
    }

    write_internal_config_flag = true; // 14/6/22 temp use flag instead of direct write internal
}
          
