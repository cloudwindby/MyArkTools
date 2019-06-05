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
#pragma comment(lib,"Version.lib")


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

#define SERVICE_NAME L"arktools"  //ServiceName
#define DISPLAY_NAME L"arktools"  //DisplayName
#define LOCAL_SYS_FILE_NAME "/arktools.sys"  //Local .sys File Name



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

#define IOCTL_HELLO_WORLD                   MY_CTL_CODE(0)
#define IOCTRL_GET_CR3                      MY_CTL_CODE(1)
#define IOCTRL_GET_SSDT_COUNT               MY_CTL_CODE(2)
#define IOCTRL_GET_SSDT_INFO                MY_CTL_CODE(3)
#define IOCTRL_GET_EPROCESS                 MY_CTL_CODE(4)
#define IOCTRL_MEMREAD                      MY_CTL_CODE(5)
#define IOCTRL_GET_ShadowSDT_COUNT          MY_CTL_CODE(6)
#define IOCTRL_GET_ShadowSSDT_INFO          MY_CTL_CODE(7)
#define IOCTL_GET_IDT                       MY_CTL_CODE(8)
#define IOCTRL_GET_ETHREAD                  MY_CTL_CODE(9)

#define IOCTRL_GET_KEYBOARD_MAJOR_FUNC      MY_CTL_CODE(10)
#define IOCTRL_GET_MOUSE_MAJOR_FUNC         MY_CTL_CODE(11)
#define IOCTRL_GET_I8042PRT_MAJOR_FUNC      MY_CTL_CODE(12)
#define IOCTRL_GET_TCPIP_MAJOR_FUNC         MY_CTL_CODE(13)


#define IOCTL_ERROR 2

#define LOADDRIVER_OK 101
#define LOADDRIVER_ERROR 102
//
// TODO: Add your IOCTL define here
//



//
// TODO: Add your struct,enum(public) define here
//

typedef struct _PROCESSINFO
{
    unsigned int CurrentPid;
    unsigned int ParentPid;
    WCHAR FilePath[256];
    unsigned int EprocessAddr;
    unsigned int DirBase;
}PROCESSINFO, *PPROCESSINFO;

typedef struct _AllPROCESSINFO
{
    unsigned int ProcessCount;
    PROCESSINFO PsInfo[1024];
}ALLPROCESSINFO, *PALLPROCESSINFO;

struct tagMemContent
{
    unsigned int nPID;		// target process ID
    unsigned int nLength;	// required data length
    unsigned int nAddress;	// data address in virtual address
    unsigned int nSize;		// capacity of pContent
    UCHAR pContent[];			// R/W contents, stored in R3
};

typedef struct _THREADINFO
{
    unsigned int ThreadID;
    unsigned int EThread;
    unsigned int TEB;
    unsigned int Win32StartAddr;
    unsigned int ContextSwitch;
    unsigned char Priority;
    unsigned char Status;
}THREADINFO,*PTHREADINFO;

/* EOF */

