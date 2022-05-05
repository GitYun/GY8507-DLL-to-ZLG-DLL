/**
 * @file warp.h
 * @brief 
 * @author yun (renqingyun@crprobot.com)
 * @date 2022-04-22
 * @version 0.1
 * @attention
 * @htmlonly
 * @endhtmlonly
 * 
 * @copyright Copyright (c) 2022 Chengdu CRP Technology Co., Ltd.
 */

#ifndef WARP_H
#define WARP_H

#include "ControlCAN.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
    DWORD (*pfn_VCI_OpenDevice)   (DWORD DeviceType, DWORD DeviceInd, DWORD Reserved);
    DWORD (*pfn_VCI_CloseDevice)  (DWORD DeviceType, DWORD DeviceInd);
    DWORD (*pfn_VCI_InitCAN)      (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig);
    DWORD (*pfn_VCI_ReadBoardInfo)(DWORD DeviceType, DWORD DeviceInd, PVCI_BOARD_INFO pInfo);
    DWORD (*pfn_VCI_ReadErrInfo)  (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_ERR_INFO pErrInfo);
    DWORD (*pfn_VCI_ReadCANStatus)(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_STATUS pCANStatus);
    DWORD (*pfn_VCI_GetReference) (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, DWORD RefType, PVOID pData);
    DWORD (*pfn_VCI_SetReference) (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, DWORD RefType, PVOID pData);
    DWORD (*pfn_VCI_ClearBuffer)  (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);
    DWORD (*pfn_VCI_StartCAN)     (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);
    DWORD (*pfn_VCI_ResetCAN)     (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);
    ULONG (*pfn_VCI_Transmit)     (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_OBJ pSend);
    ULONG (*pfn_VCI_Receive)      (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_OBJ pReceive);
    ULONG (*pfn_VCI_Receive2)     (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, BYTE *Recvbuf);

    DWORD (*pfn_VCI_FindGyUsbDevice)(PVCI_BOARD_INFO pInfo);
    DWORD (*pfn_VCI_ConnectDevice)  (DWORD DeviceType, DWORD DeviceInd);
    DWORD (*pfn_VCI_ResumeConfig)   (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);
} GY8507_funcs_t;

extern GY8507_funcs_t* GY8507_funcs_table_get(void);

#ifdef __cplusplus
}
#endif

#endif /* WARP_H */

//-----------------------------------结束-------------------------------------------------
