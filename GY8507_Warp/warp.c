/**
 * @file warp.c
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

#include "warp.h"

#if __cplusplus
extern "C" {
#endif

static GY8507_funcs_t g_GY8507_funcs_table = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

GY8507_funcs_t* GY8507_funcs_table_get(void)
{
    HMODULE dll_module = LoadLibrary(TEXT("VCI_CAN.dll"));
    if (dll_module == NULL)
    {
        return NULL;
    }

    g_GY8507_funcs_table.pfn_VCI_OpenDevice     = (DWORD (*) (DWORD, DWORD, DWORD))                   GetProcAddress(dll_module, "VCI_OpenDevice");
    g_GY8507_funcs_table.pfn_VCI_CloseDevice    = (DWORD (*) (DWORD, DWORD))                          GetProcAddress(dll_module, "VCI_CloseDevice");
    g_GY8507_funcs_table.pfn_VCI_InitCAN        = (DWORD (*) (DWORD, DWORD, DWORD, PVCI_INIT_CONFIG)) GetProcAddress(dll_module, "VCI_InitCAN");
    g_GY8507_funcs_table.pfn_VCI_ReadBoardInfo  = (DWORD (*) (DWORD, DWORD, PVCI_BOARD_INFO))         GetProcAddress(dll_module, "VCI_ReadBoardInfo");
    g_GY8507_funcs_table.pfn_VCI_ReadErrInfo    = (DWORD (*) (DWORD, DWORD, DWORD, PVCI_ERR_INFO))    GetProcAddress(dll_module, "VCI_ReadErrInfo");
    g_GY8507_funcs_table.pfn_VCI_ReadCANStatus  = (DWORD (*) (DWORD, DWORD, DWORD, PVCI_CAN_STATUS))  GetProcAddress(dll_module, "VCI_ReadCANStatus");
    g_GY8507_funcs_table.pfn_VCI_GetReference   = (DWORD (*) (DWORD, DWORD, DWORD, DWORD, PVOID))     GetProcAddress(dll_module, "VCI_GetReference");
    g_GY8507_funcs_table.pfn_VCI_SetReference   = (DWORD (*) (DWORD, DWORD, DWORD, DWORD, PVOID))     GetProcAddress(dll_module, "VCI_SetReference");
    g_GY8507_funcs_table.pfn_VCI_ClearBuffer    = (DWORD (*) (DWORD, DWORD, DWORD))                   GetProcAddress(dll_module, "VCI_ClearBuffer");
    g_GY8507_funcs_table.pfn_VCI_StartCAN       = (DWORD (*) (DWORD, DWORD, DWORD))                   GetProcAddress(dll_module, "VCI_StartCAN");
    g_GY8507_funcs_table.pfn_VCI_ResetCAN       = (DWORD (*) (DWORD, DWORD, DWORD))                   GetProcAddress(dll_module, "VCI_ResetCAN");
    g_GY8507_funcs_table.pfn_VCI_Transmit       = (ULONG (*) (DWORD, DWORD, DWORD, PVCI_CAN_OBJ))     GetProcAddress(dll_module, "VCI_Transmit");
    g_GY8507_funcs_table.pfn_VCI_Receive        = (ULONG (*) (DWORD, DWORD, DWORD, PVCI_CAN_OBJ))     GetProcAddress(dll_module, "VCI_Receive");
    g_GY8507_funcs_table.pfn_VCI_Receive2       = (ULONG (*) (DWORD, DWORD, DWORD, BYTE *))           GetProcAddress(dll_module, "VCI_Receive2");

    g_GY8507_funcs_table.pfn_VCI_FindGyUsbDevice= (DWORD (*)(PVCI_BOARD_INFO))                        GetProcAddress(dll_module, "VCI_FindGyUsbDevice");
    g_GY8507_funcs_table.pfn_VCI_ConnectDevice  = (DWORD (*)(DWORD, DWORD))                           GetProcAddress(dll_module, "VCI_ConnectDevice");
    g_GY8507_funcs_table.pfn_VCI_ResumeConfig   = (DWORD (*)(DWORD, DWORD, DWORD))                    GetProcAddress(dll_module, "VCI_ResumeConfig");

    return &g_GY8507_funcs_table;
}

#if __cplusplus
}
#endif
