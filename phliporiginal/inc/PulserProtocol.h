/****************************************************************************************
* ModeBusProtocol.H 																			   	*
* DESCRIPTION:																		   	*
*   This file contains ALL 	WMP (Water Meter Pulser) packet exported routints and definitions .  
*
*																					   	*
* HISTORY																			 	*
*   Ferlandes Yossi   15/05/2022     Created										  		*
****************************************************************************************/
#ifndef _PULSER_PROTOCOL_H
#define _PULSER_PROTOCOL_H

#undef PUBLIC
#ifdef PULSER_PROTOCOL_H
#define  PUBLIC
#else
#define  PUBLIC  extern
#endif

#pragma pack(1)

#define MAX_BYTE_IN_PACKET 200
typedef struct  {
  uint8_t Identification[4]; 
  uint8_t Stx;
  uint8_t Add[6];
  uint8_t Stx1;
  uint8_t ModelId[4];
  uint8_t MessaggeType[2];
  uint8_t Len;
}PulserHeaderStaruct;
  
typedef struct  {
  PulserHeaderStaruct header;
  uint8_t version;
  uint8_t cs;
  uint8_t Etx;
}PulserReqConnection;
typedef struct  {
  PulserHeaderStaruct header;
  uint8_t PulserId[4];
  uint8_t MeterId[6];
  uint8_t Clock[6];
  uint8_t Version;
  uint8_t Factor;
  uint8_t UpluadTime[3];
  uint8_t TotalConsumption[4];
  uint8_t LeakageEvent[3];
  uint8_t LowBattery[3];
  uint8_t cs;
  uint8_t Etx;
}PulserParamsRespStruct;

typedef struct  {
  uint8_t       version;
  uint8_t       cs;
  uint8_t       Etx;
}PulserInitReq;



typedef struct  {
  DailyData     Consumption; 
  uint8_t       cs;
  uint8_t       Etx;
}PulserDailyData;
 
  
typedef struct  {
  uint8_t Err_code;
  uint8_t PendingFunction;
  uint8_t Cs;
  uint8_t Etx;
}PulserConnectionRes;

typedef struct  {
  TimeStruct Time;
  uint8_t  MeterId[6];
  uint8_t  Factor;
  uint8_t  UploadTime[3];
  int32_t  Consumption;
  uint8_t PendingFunction;
  uint8_t Cs;
  uint8_t Etx;
}PulserIntalizaionRes;

typedef struct  {
  uint8_t EC;
  uint8_t PF;
  uint8_t CS;
  uint8_t Etx;
}PulserACK;



typedef struct  {
  TimeStruct Time;
  uint8_t MisingDays;
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;

}PulserDataRes;

typedef struct  {
  TimeStruct Time;
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;
}PulserSetTime;

typedef struct  {
  uint8_t consumption[4];
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;
}PulserSetConsumption;

typedef struct  {

  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;
}PulserUploadTime;
typedef struct  {

  uint8_t factor;
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;
}PulserSetFactor;


typedef struct  {
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;
}PulserGetLastDayDdata;


typedef struct  {
  uint8_t Year;
  uint8_t Month;
  uint8_t Day;
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;
}PulserpreviousDayDdata;


typedef struct  {
  uint16_t PacketIndex;
  uint8_t FirmwareVersion;
  uint8_t TotalFileBytes[3];
  uint16_t TotalBytesInPacket;
  uint8_t TotalPackets[3];
  uint16_t FileCS;
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;
}PulserUploadHederStruct;


typedef struct  {
  uint8_t  NewFirmwareVersion;
  uint8_t  NewTotalFileBytes[3];
  uint16_t NewTotalBytesInPacket;
  uint8_t  NewTotalPackets[3];
  uint16_t NextPacketIndex;
  uint32_t TotalBytesReceived;
  uint16_t NewFileCS;
}PulserNewFirmwareStruct;


typedef struct  {
  PulserHeaderStaruct header;
  uint16_t PacketIndex;
  uint8_t FirmwareVersion;
  uint8_t AccetUpload;
  uint16_t NextPacketIndex;
   uint8_t ErrCode;
  uint8_t cs;
  uint8_t Etx;
}PulserUploadHederResStruct;

typedef struct  {
  PulserHeaderStaruct header;
  uint16_t PacketIndex  ;
  uint8_t  AckNack;
  uint16_t NextPacketIndex;
  uint8_t TotalBytesReceived[3];
  uint8_t ErrCode;
  uint8_t cs;
  uint8_t Etx;
}PulserUploadPcketResStruct;

typedef struct  {
  uint8_t FirmwareVersion;
  uint8_t ApplyReject;
  uint8_t TotalBytesValidation[3];
  uint16_t CsValidaion;
  uint8_t  ApplyTimeType;
  uint16_t ApplyTimeValue;
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;
}PulserApplyFirmwareStruct;

typedef struct  {
  uint16_t PacketIndex;
  uint16_t BytesInPacket;
  uint8_t data[MAX_BYTE_IN_PACKET];
}PulserUploadPacketStruct;
typedef struct  {
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;
}PulserResetData;

typedef struct  {
  uint8_t MeterId[6];
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;
}PulserSetMeterId;

typedef struct  {
  uint8_t ModulId[4];
  uint8_t KeepConnection;
  uint8_t Cs;
  uint8_t Etx;
}PulserSetModuleId;


typedef struct
{
 PulserHeaderStaruct Header;
 uint8_t Info[255]; 
}PulserStaruct;


typedef enum
{
 PP_DISCONNECT_STATE = 0,
 PP_CONNECTION_REQ_STATE,
 PP_DAILY_DATA_STATE,
 PP_WAIT_FOR_INITALIZATION_STATE,
 PP_WAIT_FOR_METER_ID_STATE,
 PP_ALERT_STATE
}PulserState;


typedef enum
{
 DATA_RESP_COMMAND = 0,
 DAILY_DATA_COMMAND,
 ALERT_COMMAND,
 ACK_COMMAND,
 NACK_COMMAND,
 SET_PULSER_ID,
 SET_METER_ID,
 SET_CLOCK,
 SET_PULSER_FACTOR,
 SET_DAILY_UPLOAD_TIME,
 SET_CONSUMPTION_READING,
 GET_PARAMETERS,
 GET_LAST_DAILY_DATA,
 GET_PREVIOUS_DAILY_DATA,
 RESET_EVENT_RECORD,
 SET_LEKAGE_EVENT_TRESHOLD,
 SET_LOW_BATTERY_EVENT_TRESHOLD,
 RESET_DATA,
 RESTART_PULSER,
 UPLOAD_FIRMWARE,
 APPLY_FIRMWARE,
 INIT_REQ,
 INIT_RES,
 MAX_CODE_ID

}PulserDIname;


typedef enum
{
    CONNECTION_SUCCESS = 0,
    PULSER_ID_NOT_AUTHORIZED,
    METER_ID_NOT_AUTHORIZED,
    PULSER_ID_NOT_RECOGNIZED,
    METER_ID_NOT_RECOGNIZED
}ConnectioErrorCodes;

PUBLIC uint8_t PP_ParseRxMsg(uint8_t   *pDLTMsg, uint8_t   *pDLTDdata, uint16_t *pRxMsgLen);
PUBLIC void PP_ReqConnectionFromServer(void);
PUBLIC uint8_t PP_SendDailyData(uint8_t indexdate,uint16_t IndexDay,uint8_t *pDate);
PUBLIC uint8_t PP_GetDI(uint8_t *RxData);
PUBLIC uint8_t PP_SendACK(void);
PUBLIC uint8_t PP_SendNACK(uint8_t err);


#endif
