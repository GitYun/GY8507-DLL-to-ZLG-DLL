//#include "ControlCAN.h"

#ifndef CONTROL_CAN_H
#define CONTROL_CAN_H

#include <windows.h>

#define REFTYPE_MODE 0
#define REFTYPE_FILTER 1
#define REFTYPE_ACR0 2
#define REFTYPE_ACR1 3
#define REFTYPE_ACR2 4
#define REFTYPE_ACR3 5
#define REFTYPE_AMR0 6
#define REFTYPE_AMR1 7
#define REFTYPE_AMR2 8
#define REFTYPE_AMR3 9
#define REFTYPE_kCANBAUD 10
#define REFTYPE_TIMING0 11
#define REFTYPE_TIMING1 12
#define REFTYPE_CANRX_EN 13
#define REFTYPE_UARTBAUD 14
#define REFTYPE_ALL 15

#define REFTYPE_DEVICE_IP0 16
#define REFTYPE_DEVICE_IP1 17
#define REFTYPE_DEVICE_IP2 18
#define REFTYPE_DEVICE_IP3 19
#define REFTYPE_HOST_IP0 20
#define REFTYPE_HOST_IP1 21
#define REFTYPE_HOST_IP2 22
#define REFTYPE_HOST_IP3 23

#define REFTYPE_BORRD_INFO 32

#define REG_CAN0_MODE 0
#define REG_CAN0_FILTER 1
#define REG_CAN0_ACR0 2
#define REG_CAN0_ACR1 3
#define REG_CAN0_ACR2 4
#define REG_CAN0_ACR3 5
#define REG_CAN0_AMR0 6
#define REG_CAN0_AMR1 7
#define REG_CAN0_AMR2 8
#define REG_CAN0_AMR3 9
#define REG_CAN0_kCANBAUD 10
#define REG_CAN0_BTR0 11
#define REG_CAN0_BTR1 12
#define REG_CAN0_CANRX_EN 13
//#define REG_CAN0_EXTENDED	14
#define REG_CAN0_ALL 15

#define REG_CAN1_MODE 16
#define REG_CAN1_FILTER 17
#define REG_CAN1_ACR0 18
#define REG_CAN1_ACR1 19
#define REG_CAN1_ACR2 20
#define REG_CAN1_ACR3 21
#define REG_CAN1_AMR0 22
#define REG_CAN1_AMR1 23
#define REG_CAN1_AMR2 24
#define REG_CAN1_AMR3 25
#define REG_CAN1_kCANBAUD 26
#define REG_CAN1_BTR0 27
#define REG_CAN1_BTR1 28
#define REG_CAN1_CANRX_EN 29
#define REG_CAN1_ALL 31

#define REG_DEVICE_IP0 32
#define REG_DEVICE_IP1 33
#define REG_DEVICE_IP2 34
#define REG_DEVICE_IP3 35
#define REG_HOST_IP0 36
#define REG_HOST_IP1 37
#define REG_HOST_IP2 38
#define REG_HOST_IP3 39
#define REG_DEV_UDP_PORTH 40 // 40 41
#define REG_DEV_UDP_PORTL 41
#define REG_HOST_UDP_PORTH 42 // 42,43
#define REG_HOST_UDP_PORTL 43
#define REG_HOST_UDP_PORTL 43

#define REG_CAN0_EXTEND 45
#define REG_CAN0_SENDID0 46
#define REG_CAN0_SENDID1 47
#define REG_CAN0_SENDID2 48
#define REG_CAN0_SENDID3 49
#define REG_CAN1_EXTEND 50
#define REG_CAN1_SENDID0 51
#define REG_CAN1_SENDID1 52
#define REG_CAN1_SENDID2 53
#define REG_CAN1_SENDID3 54

//接口卡类型定义
#define DEV_CAN232B 1
#define DEV_USBCAN 2
#define DEV_USBCAN200 3
#define DEV_NETCAN 4
#define DEV_NETCAN200 5
#define DEV_PCICAN 6
#define DEV_PCICAN200 7

//函数调用返回状态值
#define STATUS_OK 1
#define STATUS_ERR1 0  //操作失败
#define STATUS_ERR2 -1 //设备未打开

// 1.GY85XX系列CAN接口卡信息的数据类型。
typedef struct _VCI_BOARD_INFO
{
    USHORT hw_Version;
    USHORT fw_Version;
    USHORT dr_Version;
    USHORT in_Version;
    USHORT irq_Num;
    BYTE can_Num;
    CHAR str_Serial_Num[8];
    CHAR str_hw_Type[16];
    char str_GYUsb_Serial[4][4];
} VCI_BOARD_INFO, *PVCI_BOARD_INFO; //共63个字节

// 2.定义CAN信息帧的数据类型。
typedef struct _VCI_CAN_OBJ
{
    // ID[0] [7:0]bits ==> 11bits CAN-ID [10:3]bits
    // ID[1] [7:5]bits ==> 11bits CAN-ID [ 2:0]bits
    // or
    // ID[0] [7:0]bits ==> 29bits CAN-ID [28:21]bits
    // ID[1] [7:0]bits ==> 29bits CAN-ID [20:13]bits
    // ID[2] [7:0]bits ==> 29bits CAN-ID [12: 5]bits
    // ID[3] [7:3]bits ==> 29Bits CAN-ID [ 4: 0]bits
    BYTE ID[4];
    UINT TimeStamp;	 //保留不用
    BYTE TimeFlag;	 //保留不用
    BYTE SendType;	 //保留不用
    BYTE RemoteFlag; //是否是远程帧
    BYTE ExternFlag; //是否是扩展帧
    BYTE DataLen;
    BYTE Data[8];
    BYTE Reserved[3];
} VCI_CAN_OBJ, *PVCI_CAN_OBJ;

// 3.定义CAN控制器状态的数据类型。
typedef struct _VCI_CAN_STATUS
{
    UCHAR ErrInterrupt;
    UCHAR regMode;
    UCHAR regStatus;
    UCHAR regALCapture;
    UCHAR regECCapture;
    UCHAR regEWLimit;
    UCHAR regRECounter;
    UCHAR regTECounter;
    DWORD Reserved;
} VCI_CAN_STATUS, *PVCI_CAN_STATUS;

// 4.定义错误信息的数据类型。
typedef struct _ERR_INFO
{
    UINT ErrCode;
    BYTE Passive_ErrData[3];
    BYTE ArLost_ErrData;
} VCI_ERR_INFO, *PVCI_ERR_INFO;

// 5.定义初始化CAN的数据类型
typedef struct _INIT_CONFIG
{
    DWORD AccCode;
    DWORD AccMask;
    DWORD Reserved;
    UCHAR Filter;
    UCHAR kCanBaud;
    UCHAR Timing0;
    UCHAR Timing1;
    UCHAR Mode;
    UCHAR CanRx_IER; // added by yyd
} VCI_INIT_CONFIG, *PVCI_INIT_CONFIG;

// PCI_CAN接收回调函数形式
typedef void (*RCVCALLBACK)(DWORD, DWORD, VCI_CAN_OBJ *, LPVOID);

//专为打开NET_CAN设计
// extern __declspec(dllexport) DWORD VCI_OpenDevice(DWORD DeviceType, DWORD DeviceIP, DWORD DevicePort, DWORD HostPort);

#if __cplusplus
extern "C" {
#endif

// #define STDCALL __stdcall
#define STDCALL

#define EXTERN extern

// 有没有以下函数声明都无所谓

EXTERN DWORD STDCALL VCI_OpenDevice(DWORD DeviceType, DWORD DeviceInd, DWORD Reserved);
EXTERN DWORD STDCALL VCI_CloseDevice(DWORD DeviceType, DWORD DeviceInd);
EXTERN DWORD STDCALL VCI_InitCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig);

EXTERN DWORD STDCALL VCI_ResumeConfig(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);

EXTERN DWORD STDCALL VCI_ReadBoardInfo(DWORD DeviceType, DWORD DeviceInd, PVCI_BOARD_INFO pInfo);
EXTERN DWORD STDCALL VCI_ReadErrInfo(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_ERR_INFO pErrInfo);
EXTERN DWORD STDCALL VCI_ReadCANStatus(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_STATUS pCANStatus);

EXTERN DWORD STDCALL VCI_GetReference(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, DWORD RefType, BYTE *pData);
EXTERN DWORD STDCALL VCI_SetReference(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, DWORD RefType, BYTE *pData);

EXTERN DWORD STDCALL VCI_ClearBuffer(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);

EXTERN DWORD STDCALL VCI_StartCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);
EXTERN DWORD STDCALL VCI_ResetCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);

EXTERN DWORD STDCALL VCI_Transmit(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_OBJ pSend);
EXTERN DWORD STDCALL VCI_Receive(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_OBJ pReceive);
EXTERN DWORD STDCALL VCI_Receive2(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, BYTE *Recvbuf); //功能同receive函数，未采用结构体。

EXTERN DWORD STDCALL VCI_FindGyUsbDevice(PVCI_BOARD_INFO pInfo);
EXTERN DWORD STDCALL VCI_UsbDeviceReset(DWORD DeviceType, DWORD DeviceInd, DWORD Reserved);

// Below 4 functions for GY784X PCI-CAN Interrupt Receive.
EXTERN DWORD STDCALL VCI_CANSetCallBack(DWORD DeviceIndex, RCVCALLBACK RecieveCallback, LPVOID lpUser);
EXTERN DWORD STDCALL VCI_CANExitCallBack(DWORD DeviceIndex);
EXTERN DWORD STDCALL VCI_CANStartCallBack(DWORD DeviceIndex);
EXTERN DWORD STDCALL VCI_CANStopCallBack(DWORD DeviceIndex);

EXTERN DWORD STDCALL VCI_ConnectDevice(DWORD DeviceType, DWORD DeviceInd); //查询设备是否在系统中，仅针对USB-CAN设备

#if __cplusplus
}
#endif

#endif // CONTROL_CAN_H
