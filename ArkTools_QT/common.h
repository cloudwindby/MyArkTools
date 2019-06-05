/**************************************************************************************
* AUTHOR : Liu
* DATE   : 2019-4-20
* MODULE : common.h
*
* Command: 
*	IOCTRL Common Header
*
* Description:
*	Common data for the IoCtrl driver and application
*
****************************************************************************************
* Copyright (C) 2010 Liu.
****************************************************************************************/

#pragma once 

#pragma comment(lib,"Advapi32.lib")

//#######################################################################################
// D E F I N E S
//#######################################################################################

#if DBG
#define dprintf DbgPrint
#else
#define dprintf
#endif

//不支持符号链接用户相关性
#define DEVICE_NAME                  L"\\Device\\devArkTools"             // Driver Name
#define SYMBOLIC_LINK_NAME           L"\\DosDevices\\ArkTools"            // Symbolic Link Name
#define WIN32_LINK_NAME              L"\\\\.\\ArkTools"                    // Win32 Link Name


//支持符号链接用户相关性
#define SYMBOLIC_LINK_GLOBAL_NAME    L"\\DosDevices\\Global\\ArkTools"    // Symbolic Link Name

#define DATA_TO_APP                  "Hello World from Driver"

//
// Device IO Control Codes
//
#define IOCTL_BASE          0x800
#define MY_CTL_CODE(i)        \
    CTL_CODE                  \
    (                         \
        FILE_DEVICE_UNKNOWN,  \
        IOCTL_BASE + i,       \
        METHOD_BUFFERED,      \
        FILE_ANY_ACCESS       \
    )

#define IOCTL_HELLO_WORLD            MY_CTL_CODE(0)
#define IOCTRL_GET_CR3               MY_CTL_CODE(1)
#define IOCTRL_SEND_TO_APP           MY_CTL_CODE(2)

#define CREATE_DEVICE_ERROR 0
#define CREATE_DEVICE_SUCCESS 1
#define IOCTL_ERROR 2
#define OPENSCMANAGER_ERROR 3
#define CREATESERVICE_ERROR 4
#define CREATESERVICE_OK 5
#define STARTSERVICE_ERROR 6
#define STARTSERVICE_OK 7

//
// TODO: Add your IOCTL define here
//



//
// TODO: Add your struct,enum(public) define here
//



/* EOF */

