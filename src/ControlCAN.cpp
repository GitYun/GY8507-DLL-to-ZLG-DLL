/**
 * @file ControlCAN.cpp
 * @brief 
 * @author yun (renqingyun@crprobot.com)
 * @date 2022-04-21
 * @version 0.1
 * @attention
 * @htmlonly
 * @endhtmlonly
 * 
 * @copyright Copyright (c) 2022 Chengdu CRP Technology Co., Ltd.
 */

#include "ControlCAN.h"

// extern "C"
// {

typedef struct _GY8507_VCI_INIT_CONFIG {
    DWORD AccCode;
    DWORD AccMask;
    DWORD Reserved;
    UCHAR Filter;   // 过滤方式, 0: 单滤波, 1: 双滤波
    UCHAR kCanBaud; // CAN波特率索引号
    UCHAR Timing0;
    UCHAR Timing1;
    UCHAR Mode;
    UCHAR CanRx_IER;
} GY8507_VCI_INIT_CONFIG, *GY8507_PVCI_INIT_CONFIG;

typedef struct _GY8507_VCI_CAN_OBJ
{
    BYTE ID[4];
    UINT TimeStamp;	 //保留不用
    BYTE TimeFlag;	 //保留不用
    BYTE SendType;	 //保留不用
    BYTE RemoteFlag; //是否是远程帧
    BYTE ExternFlag; //是否是扩展帧
    BYTE DataLen;
    BYTE Data[8];
    BYTE Reserved[3];
} GY8507_VCI_CAN_OBJ, *GY8507_PVCI_CAN_OBJ;

typedef struct {
    DWORD (*pfn_VCI_OpenDevice)   (DWORD DeviceType, DWORD DeviceInd, DWORD Reserved);
    DWORD (*pfn_VCI_CloseDevice)  (DWORD DeviceType, DWORD DeviceInd);
    DWORD (*pfn_VCI_InitCAN)      (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, GY8507_PVCI_INIT_CONFIG pInitConfig);
    DWORD (*pfn_VCI_ReadBoardInfo)(DWORD DeviceType, DWORD DeviceInd, PVCI_BOARD_INFO pInfo);
    DWORD (*pfn_VCI_ReadErrInfo)  (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_ERR_INFO pErrInfo);
    DWORD (*pfn_VCI_ReadCANStatus)(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_STATUS pCANStatus);
    DWORD (*pfn_VCI_GetReference) (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, DWORD RefType, PVOID pData);
    DWORD (*pfn_VCI_SetReference) (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, DWORD RefType, PVOID pData);
    DWORD (*pfn_VCI_ClearBuffer)  (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);
    DWORD (*pfn_VCI_StartCAN)     (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);
    DWORD (*pfn_VCI_ResetCAN)     (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);
    ULONG (*pfn_VCI_Transmit)     (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, GY8507_PVCI_CAN_OBJ pSend);
    ULONG (*pfn_VCI_Receive)      (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, GY8507_PVCI_CAN_OBJ pReceive);
    ULONG (*pfn_VCI_Receive2)     (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, BYTE *Recvbuf);

    DWORD (*pfn_VCI_FindGyUsbDevice)(PVCI_BOARD_INFO pInfo);
    DWORD (*pfn_VCI_ConnectDevice)  (DWORD DeviceType, DWORD DeviceInd);
    DWORD (*pfn_VCI_ResumeConfig)   (DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);
} GY8507_funcs_t;

extern "C" GY8507_funcs_t* GY8507_funcs_table_get(void);

static GY8507_funcs_t *g_GY8507_funcs = nullptr;

EXTERNC DWORD STDCALL VCI_OpenDevice(DWORD DeviceType, DWORD DeviceInd, DWORD Reserved)
{
    DWORD ret = 0;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    ret = g_GY8507_funcs->pfn_VCI_OpenDevice(DeviceType, DeviceInd, Reserved);

    return ret == 1 ? 1 : 0;
}

DWORD STDCALL VCI_CloseDevice(DWORD DeviceType, DWORD DeviceInd)
{
    DWORD ret = 0;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    ret = g_GY8507_funcs->pfn_VCI_CloseDevice(DeviceType, DeviceInd);

    return ret == 1 ? 1 : 0;
}

DWORD STDCALL VCI_InitCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig)
{
    DWORD ret = 0;
    GY8507_VCI_INIT_CONFIG GY8507_InitConfig;
    BYTE tim0_tim1_baud_table[][2] = {
        {0x00, 0x00}, //  0: none
        {0xBF, 0xFF}, //  1: 5Kbps
        {0x31, 0x1C}, //  2: 10Kbps
        {0x18, 0x1C}, //  3: 20Kbps
        {0x87, 0xFF}, //  4: 40Kbps
        {0x09, 0x1C}, //  5: 50Kbps
        {0x83, 0xFF}, //  6: 80Kbps
        {0x04, 0x1C}, //  7: 100Kbps
        {0x03, 0x1C}, //  8: 125Kbps
        {0x81, 0xFA}, //  9: 200Kbps
        {0x01, 0x1C}, // 10: 250Kbps
        {0x80, 0xFA}, // 11: 400Kbpx
        {0x00, 0x1C}, // 12: 500Kbpx
        {0x80, 0xB6}, // 13: 666Kbps
        {0x00, 0x16}, // 14: 800Kbps
        {0x00, 0x14}, // 15: 10000Kbps
    };

    GY8507_InitConfig.AccCode = pInitConfig->AccCode;
    GY8507_InitConfig.AccMask = pInitConfig->AccMask;
    GY8507_InitConfig.Reserved = pInitConfig->Reserved;
    GY8507_InitConfig.Filter = !pInitConfig->Filter;
    GY8507_InitConfig.Timing0 = pInitConfig->Timing0;
    GY8507_InitConfig.Timing1 = pInitConfig->Timing1;

    GY8507_InitConfig.CanRx_IER = 1;

    for (BYTE i = 1; i < sizeof(tim0_tim1_baud_table)/sizeof(tim0_tim1_baud_table[0]); ++i)
    {
        if (tim0_tim1_baud_table[i][0] == pInitConfig->Timing0
            && tim0_tim1_baud_table[i][1] == pInitConfig->Timing1)
        {
            GY8507_InitConfig.kCanBaud = i;
            break;
        }
    }

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    ret = g_GY8507_funcs->pfn_VCI_InitCAN(DeviceType, DeviceInd, CANInd, &GY8507_InitConfig);

    return ret == 1 ? 1 : 0;
}

DWORD STDCALL VCI_ReadBoardInfo(DWORD DeviceType, DWORD DeviceInd, PVCI_BOARD_INFO pInfo)
{
    DWORD ret = 0;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    ret = g_GY8507_funcs->pfn_VCI_ReadBoardInfo(DeviceType, DeviceInd, pInfo);
    if (ret != 1)
    {
        ret = g_GY8507_funcs->pfn_VCI_FindGyUsbDevice(pInfo);
    }

    return ret == 0 ? 0 : 1;
}

DWORD STDCALL VCI_ReadErrInfo(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_ERR_INFO pErrInfo)
{
    DWORD ret = 0;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    ret =  g_GY8507_funcs->pfn_VCI_ReadErrInfo(DeviceType, DeviceInd, CANInd, pErrInfo);

    return ret == 1 ? 1 : 0;
}

DWORD STDCALL VCI_ReadCANStatus(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_STATUS pCANStatus)
{
    DWORD ret = 0;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    ret = g_GY8507_funcs->pfn_VCI_ReadCANStatus(DeviceType, DeviceInd, CANInd, pCANStatus);

    return ret == 1 ? 1 : 0;
}

DWORD STDCALL VCI_GetReference(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, DWORD RefType, PVOID pData)
{
    DWORD ret = 0;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    ret = g_GY8507_funcs->pfn_VCI_GetReference(DeviceType, DeviceInd, CANInd, RefType, pData);

    return ret == 1 ? 1 : 0;
}

DWORD STDCALL VCI_SetReference(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, DWORD RefType, PVOID pData)
{
    DWORD ret = 0;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    ret = g_GY8507_funcs->pfn_VCI_SetReference(DeviceType, DeviceInd, CANInd, RefType, pData);

    return ret == 1 ? 1 : 0;
}

ULONG STDCALL VCI_GetReceiveNum(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd)
{
    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    return 300;
}

DWORD STDCALL VCI_ClearBuffer(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd)
{
    DWORD ret = 0;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    ret = g_GY8507_funcs->pfn_VCI_ClearBuffer(DeviceType, DeviceInd, CANInd);

    return ret == 1 ? 1 : 0;
}

DWORD STDCALL VCI_StartCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd)
{
    DWORD ret = 0;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    ret = g_GY8507_funcs->pfn_VCI_StartCAN(DeviceType, DeviceInd, CANInd);

    return ret == 1 ? 1 : 0;
}

DWORD STDCALL VCI_ResetCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd)
{
    DWORD ret = 0;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    ret = g_GY8507_funcs->pfn_VCI_ResetCAN(DeviceType, DeviceInd, CANInd);

    return ret == 1 ? 1 : 0;
}

ULONG STDCALL VCI_Transmit(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_OBJ pSend, ULONG Len)
{
    ULONG rc = 0;
    ULONG ret = 0;
    GY8507_PVCI_CAN_OBJ pbuf;
    VCI_CAN_OBJ send_buf = {0};

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    for (ULONG i = 0; i < Len; ++i)
    {
        send_buf = pSend[i];

        // ZLG CAN-ID ==> GY8507 CAN-ID
        // ZLG 右对齐, GY8507 左对齐
        if (pSend[i].ExternFlag == 1)
        {
            send_buf.ID = 0
                | (((send_buf.ID >>  0) & 0x1F) << 27)  // GY8507 29bits CAN-ID [ 4: 0]bits
                | (((send_buf.ID >>  5) & 0xFF) << 16)  // GY8507 29bits CAN-ID [12: 5]bits
                | (((send_buf.ID >> 13) & 0xFF) <<  8)  // GY8507 29bits CAN-ID [20:13]bits
                | (((send_buf.ID >> 21) & 0xFF) <<  0); // GY8507 29bits CAN-ID [28:21]bits
        }
        else
        {
            send_buf.ID = 0
                | (((send_buf.ID >> 0) & 0x07) << 13)  // GY8507 11bits CAN-ID [ 2:0]bits
                | (((send_buf.ID >> 3) & 0xFF) <<  0); // GY8507 11bits CAN-ID [10:3]bits
        }

        pbuf = (GY8507_PVCI_CAN_OBJ)&send_buf;

        ret = g_GY8507_funcs->pfn_VCI_Transmit(DeviceType, DeviceInd, CANInd, pbuf);
        if (ret == 1)
        {
            rc++;
        }
        else if (ret == -1 || ret > 1)
        {
            break;
        }
    }

    return rc;
}

ULONG STDCALL VCI_Receive(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_OBJ pReceive, ULONG Len, INT WaitTime)
{
    ULONG rc = 0;
    GY8507_PVCI_CAN_OBJ pbuf = (GY8507_PVCI_CAN_OBJ)pReceive;
    PVCI_CAN_OBJ rx_buf = pReceive;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    rc = g_GY8507_funcs->pfn_VCI_Receive(DeviceType, DeviceInd, CANInd, pbuf);

    for (INT i = 0; i < rc && i < Len; ++i)
    {
        rx_buf += i;
        
        if (pbuf[i].ExternFlag == 1)
        {
            rx_buf->ID = 0
                | ((UINT)pbuf[i].ID[0] << 21)
                | ((UINT)pbuf[i].ID[1] << 13)
                | ((UINT)pbuf[i].ID[2] <<  5)
                | ((UINT)pbuf[i].ID[3] >>  3);
        }
        else
        {
            rx_buf->ID = 0
                | ((UINT)pbuf[i].ID[0] << 3)
                | ((UINT)pbuf[i].ID[1] >> 5);
        }
    }

    return rc;
}

ULONG STDCALL VCI_Receive2(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, BYTE* pReceive)
{
    ULONG rc = 0;
    UINT *can_id = NULL;

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    rc = g_GY8507_funcs->pfn_VCI_Receive2(DeviceType, DeviceInd, CANInd, pReceive);

    for (INT i = 0, idx = 0; i < rc / 13; ++i)
    {
        idx = i * 13;
        can_id = (UINT *)&(pReceive[idx + 1]);

        if ((pReceive[0] & 0x80) == 0x80) // Extended CAN-ID
        {
            *can_id = 0
                | ((UINT)pReceive[idx + 1] << 21)
                | ((UINT)pReceive[idx + 2] << 13)
                | ((UINT)pReceive[idx + 3] <<  5)
                | ((UINT)pReceive[idx + 4] >>  3);
        }
        else
        {
            *can_id = 0
                | ((UINT)pReceive[idx + 1] << 3)
                | ((UINT)pReceive[idx + 2] >> 5);
        }
    }

    return rc / 13;
}

// }