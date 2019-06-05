#ifndef CXX_ARKTOOLS_H
#include "ArkTools.h"
#endif

extern void* KeServiceDescriptorTable;

NTSTATUS
DriverEntry(IN PDRIVER_OBJECT pDriverObj, IN PUNICODE_STRING pRegistryString)
{
    NTSTATUS		status = STATUS_SUCCESS;
    UNICODE_STRING  ustrLinkName;
    UNICODE_STRING  ustrDevName;
    PDEVICE_OBJECT  pDevObj;
    int i = 0;

    dprintf("[ArkTools] EasySys Sample Driver\r\n"
        "[ArkTools] Compiled %s %s\r\n[ArkTools] In DriverEntry : %wZ\r\n",
        __DATE__, __TIME__, pRegistryString);

    // Register dispatch routines
    /*
        for(i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
        {
        pDriverObj->MajorFunction[i] = DispatchCommon;
        }
        */
    pDriverObj->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
    pDriverObj->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;

    // Dispatch routine for communications
    pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;

    // Unload routine
    pDriverObj->DriverUnload = DriverUnload;

    // Initialize the device name.
    RtlInitUnicodeString(&ustrDevName, DEVICE_NAME);

    // Create the device object and device extension
    status = IoCreateDevice(pDriverObj,
        0,
        &ustrDevName,
        FILE_DEVICE_UNKNOWN,
        0,
        FALSE,
        &pDevObj);

    if (!NT_SUCCESS(status))
    {
        dprintf("[ArkTools] Error, IoCreateDevice = 0x%x\r\n", status);
        return status;
    }

    //// Get a pointer to our device extension
    //deviceExtension = (PDEVICE_EXTENSION) deviceObject->DeviceExtension;

    //// Save a pointer to the device object
    //deviceExtension->DeviceObject = deviceObject;

    if (IoIsWdmVersionAvailable(1, 0x10))
    {
        //如果是支持符号链接用户相关性的系统
        RtlInitUnicodeString(&ustrLinkName, SYMBOLIC_LINK_GLOBAL_NAME);
    }
    else
    {
        //不支持
        RtlInitUnicodeString(&ustrLinkName, SYMBOLIC_LINK_NAME);
    }

    // Create a symbolic link to allow USER applications to access it. 
    status = IoCreateSymbolicLink(&ustrLinkName, &ustrDevName);

    if (!NT_SUCCESS(status))
    {
        dprintf("[ArkTools] Error, IoCreateSymbolicLink = 0x%x\r\n", status);

        IoDeleteDevice(pDevObj);
        return status;
    }

    //
    //	TODO: Add initialization code here.
    //

    //// Tell the I/O Manger to do BUFFERED IO
    pDevObj->Flags |= DO_BUFFERED_IO;

    dprintf("[ArkTools] DriverEntry Success\r\n");

    return STATUS_SUCCESS;
}

VOID
DriverUnload(IN PDRIVER_OBJECT pDriverObj)
{
    UNICODE_STRING strLink;

    // Unloading - no resources to free so just return.
    dprintf("[ArkTools] Unloading...\r\n");;

    //
    // TODO: Add uninstall code here.
    //

    // Delete the symbolic link
    RtlInitUnicodeString(&strLink, SYMBOLIC_LINK_NAME);
    IoDeleteSymbolicLink(&strLink);

    // Delete the DeviceObject
    IoDeleteDevice(pDriverObj->DeviceObject);

    dprintf("[ArkTools] Unloaded Success\r\n");

    return;
}

NTSTATUS
DispatchCreate(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
    pIrp->IoStatus.Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = 0;

    dprintf("[ArkTools] IRP_MJ_CREATE\r\n");

    IoCompleteRequest(pIrp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}


NTSTATUS
DispatchClose(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
    pIrp->IoStatus.Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = 0;

    dprintf("[ArkTools] IRP_MJ_CLOSE\r\n");

    IoCompleteRequest(pIrp, IO_NO_INCREMENT);

    // Return success
    return STATUS_SUCCESS;
}

NTSTATUS
DispatchCommon(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
    pIrp->IoStatus.Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = 0L;

    dprintf("[ArkTools] Common Dispatch\r\n");

    IoCompleteRequest(pIrp, 0);

    // Return success
    return STATUS_SUCCESS;
}

NTSTATUS
DispatchDeviceControl(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;	 // STATUS_UNSUCCESSFUL
    PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    ULONG uIoControlCode = 0;
    PVOID pIoBuffer = NULL;
    ULONG uInSize = 0;
    ULONG uOutSize = 0;
    ULONG buildNumber = 0;
    ULONG uByte = 0;

    //Gdt
    char szGDT[6] = { 0 };
    char *pGDTAddr = NULL;
    UINT32 uGDTSize = 0;

    //IDT
    char szIDT[6] = { 0 };
    char *pIDTAddr = NULL;
    UINT32 uIDTSize = 0;

    //CR3
    int nCR3 = 0;

    //SSDT
    int nSSDTCount = 0;
    int *pSSDTBase = NULL;

    //SSSDT
    int nSSSDTCount = 0;
    int *pSSSDTBase = NULL;

    //ProcessInfo
    int nCurrentProcessor = 0;
    PALLPROCESSINFO pProcessInfo = NULL;
    char* process = NULL;
    char* end_process = NULL;
    ULONG SEAuditValue;
    PULONG pNameInfo;
    PUNICODE_STRING lpPath;
    struct tagMemContent* pMem = NULL;
    //ProcessCount
    int i = 0;

    //ThreadInfo
    char* pEPROCESSAddr = NULL;
    char* pThread = NULL;
    char* pEndThread = NULL;
    PETHREAD ptagEthread = NULL;

    PsGetVersion(NULL, NULL, &buildNumber, NULL);
    if (buildNumber != 2600)
    {
        dprintf("[ArkTools] DispatchDeviceControl buildNumber != 2600 file:%s line:%d\n",
            __FILE__, __LINE__);
        return STATUS_UNSUCCESSFUL;
    }
    dprintf("[ArkTools] DispatchDeviceControl buildNumber:%d\n", buildNumber);

    // Get the IoCtrl Code
    uIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;

    pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
    uInSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    uOutSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

    switch (uIoControlCode)
    {

    case IOCTL_HELLO_WORLD:
    {

        nCurrentProcessor = *(int*)pIoBuffer;
        dprintf("[ArkTools]%d\r\n", nCurrentProcessor);
        KeSetSystemAffinityThread((KAFFINITY)(1 << nCurrentProcessor));
        __asm
        {
            sgdt szGDT
        }
        uGDTSize = *(SHORT*)szGDT;
        pGDTAddr = *(int*)(szGDT + 2);
        //dprintf("[ArkTools]Count:%d,GDT:%p\r\n", uGDTSize / 8, pGDTAddr);
        //dprintf("[ArkTools]SystemBuffer:%p");
        //DbgBreakPoint(); 

        //ProbeForWrite(pIoBuffer, uGDTSize, 1);
        RtlCopyMemory(pIoBuffer, pGDTAddr, uGDTSize);
        status = STATUS_SUCCESS;
        //dprintf("[ArkTools] uOutSize:%d!\r\n", uOutSize);

        //DbgBreakPoint();
        // Return success

    }
    break;

    case IOCTL_GET_IDT:
    {
        nCurrentProcessor = *(int*)pIoBuffer;
        dprintf("[ArkTools]%d\r\n", nCurrentProcessor);
        KeSetSystemAffinityThread((KAFFINITY)(1 << nCurrentProcessor));
        __asm
        {
            sidt szIDT
        }
        uIDTSize = *(SHORT*)szIDT;
        pIDTAddr = *(int*)(szIDT + 2);
        //dprintf("[ArkTools]Count:%d,GDT:%p\r\n", uGDTSize / 8, pGDTAddr);
        //dprintf("[ArkTools]SystemBuffer:%p");
        //DbgBreakPoint(); 

        //ProbeForWrite(pIoBuffer, uGDTSize, 1);
        RtlCopyMemory(pIoBuffer, pIDTAddr, uIDTSize);
        status = STATUS_SUCCESS;
    }
    break;

    case IOCTRL_GET_CR3:
    {
        // Receive data form Application
        //dprintf("[ArkTools] IOCTRL_GET_CR3\r\n");

        // Do we have any data?
        //__asm
        //{
        //    //int 3
        //    mov eax,CR3
        //    mov nCR3,eax
        //}
        //dprintf("[ArkTools] nCR3:%p\r\n", nCR3);
        //if (MmIsAddressValid((char*)nCR3) == TRUE)
        //{
        //    RtlCopyMemory(pIoBuffer, (char*)nCR3, 4096);
        //}
        //else
        //{
        //    dprintf("[ArkTools] MmIsAddressValid:false\r\n");
        //    status = STATUS_PAGE_FAULT_COPY_ON_WRITE;
        //    uOutSize = 0;
        //}

        if (uInSize > 0)
        {
            dprintf("[ArkTools] Get Data from App: %s\r\n", pIoBuffer);
            break;
        }

        // Return success
        status = STATUS_SUCCESS;
    }
    break;

    case IOCTRL_GET_SSDT_COUNT:
    {
        // Send data to Application
        dprintf("[ArkTools] IOCTRL_GET_SSDT_COUNT:%d\r\n", *(int*)((char*)KeServiceDescriptorTable + 8));
        dprintf("[ArkTools] IOCTRL_ServiceTableBase:%p\r\n", *(int*)KeServiceDescriptorTable);
        dprintf("[ArkTools] IOCTRL_SSDTParamTableBase:%p\r\n", *(int*)((char*)KeServiceDescriptorTable + 0xc));

        *(int*)pIoBuffer = *((int*)KeServiceDescriptorTable + 2);
        //KeServiceDescriptorTable;
        status = STATUS_SUCCESS;
    }
    break;

    case IOCTRL_GET_SSDT_INFO:
    {

        nSSDTCount = *(unsigned int*)((char*)KeServiceDescriptorTable + 8);
        pSSDTBase = *(unsigned int**)(KeServiceDescriptorTable);
        dprintf("[ArkTools] IOCTRL_pSSDTBase:%p\r\n", pSSDTBase);
        for (i = 0; i < nSSDTCount; i++)
        {
            ((unsigned int*)pIoBuffer)[i] = *(pSSDTBase + i);
        }
        status = STATUS_SUCCESS;
    }
    break;

    case IOCTRL_GET_ShadowSDT_COUNT:
    {
        dprintf("[ArkTools] IOCTRL_GET_ShadowSSDT_COUNT:%d\r\n", *(int*)((char*)KeServiceDescriptorTable - 0x30 + 8));
        dprintf("[ArkTools] IOCTRL_GET_ShadowSSDTBase:%p\r\n", *(int*)KeServiceDescriptorTable - 0x30);
        dprintf("[ArkTools] IOCTRL_GET_ShadowSSDTParamTableBase:%p\r\n", *(int*)((char*)KeServiceDescriptorTable - 0x30 + 0xc));

        *(int*)pIoBuffer = *(int*)((char*)KeServiceDescriptorTable - 0x30 + 8);

        status = STATUS_SUCCESS;
    }
    break;

    case IOCTRL_GET_ShadowSSDT_INFO:
    {
        nSSSDTCount = *(int*)((char*)KeServiceDescriptorTable - 0x30 + 8);
        pSSSDTBase = *(unsigned int**)((int*)KeServiceDescriptorTable - 12);
        dprintf("[ArkTools] IOCTRL_pSSSDTBase:%p\r\n", pSSSDTBase);
        for (i = 0; i < nSSSDTCount; i++)
        {
            ((unsigned int*)pIoBuffer)[i] = *(pSSSDTBase + i);
        }
        status = STATUS_SUCCESS;
    }
    break;

    case IOCTRL_GET_EPROCESS:
    {
        pProcessInfo = (PALLPROCESSINFO)pIoBuffer;
        dprintf("[ArkTools] IOCTRL_GET_EPROCESS\r\n");

        __try
        {
            __asm
            {
                mov eax, fs:[124h]; //ETHREAD
                mov eax, [eax + 44h]  //EPROCESS
                    mov process, eax
            }

            end_process = process;
            do
            {
                if (MmIsAddressValid(process) == TRUE)
                {
                    //dprintf("[ArkTools] MmIsAddressValid(process) == TRUE \n");
                    process = (char*)(*(int*)(process + 0x88) - 0x88);
                }
                else
                {
                    continue;
                }
                pProcessInfo->PsInfo[i].DirBase = *(int*)(process + 0x18);
                if (*(int*)(process + 0x84) > 0x80000000)
                {
                    pProcessInfo->PsInfo[i].CurrentPid = 0;
                    pProcessInfo->PsInfo[i].ParentPid = 0;
                    //RtlFillMemory(pProcessInfo->PsInfo[i].FilePath, sizeof(WCHAR) * 256, 0);
                    //dprintf("[ArkTools] Idle: %p,EPROCESS_:%p\r\n", *(int*)(process + 0x84), process);
                }
                else if (*(int*)(process + 0x84) == 4)
                {
                    //RtlFillMemory(pProcessInfo->PsInfo[i].FilePath,sizeof(WCHAR)*256,0);
                    pProcessInfo->PsInfo[i].CurrentPid = 4;
                    pProcessInfo->PsInfo[i].ParentPid = 0;
                    //dprintf("[ArkTools] System: %p,EPROCESS_:%p\r\n", *(int*)(process + 0x84), process);
                }
                else
                {
                    pProcessInfo->PsInfo[i].CurrentPid = *(int*)(process + 0x84);
                    //获取 _SE_AUDIT_PROCESS_CREATION_INFO
                    SEAuditValue = *(PULONG)((ULONG)process + 0x1f4);
                    //获取_OBJECT_NAME_INFORMATION指针
                    pNameInfo = (PULONG)SEAuditValue;
                    lpPath = (PUNICODE_STRING)(PVOID)pNameInfo;
                    //KdPrint(("[ArkTools] EPROCESS_FULLPATH:%wZ\n", lpPath));
                    pProcessInfo->PsInfo[i].ParentPid = *(int*)(process + 0x14c);
                    RtlCopyMemory(pProcessInfo->PsInfo[i].FilePath, lpPath->Buffer, lpPath->MaximumLength);
                    //KdPrint(("[ArkTools] EPROCESS_FULLPATH:%wZ\n", pProcessInfo->PsInfo[i].FilePath));
                    //dprintf("[ArkTools] CurrentPid: %p,EPROCESS_:%p\r\n", *(int*)(process + 0x84), process);
                }


                pProcessInfo->PsInfo[i].EprocessAddr = (int*)(process);

                i++;
            } while (process != end_process);

            pProcessInfo->ProcessCount = i;
            uOutSize = (i - 1) * sizeof(PROCESSINFO) + 4;
            status = STATUS_SUCCESS;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            dprintf("[ArkTools_Ring0] IOCTRL_GET_EPROCESS Error: File:%s,Line:%d!\r\n", __FILE__, __LINE__);
            status = STATUS_UNSUCCESSFUL;
        }

    }
    break;
    case IOCTRL_MEMREAD:	// read from memory
        // input: tagMemContent*
        // output: tagMemContent*
    {
        __try
        {
            if (!MmIsAddressValid(pIoBuffer))
            {
                dprintf("[ArkTools_Ring0] IOCTRL_MEMREAD Invalid Param file:%s line:%d\n",
                    __FILE__, __LINE__);
                status = STATUS_INVALID_PARAMETER;
            }
            pMem = pIoBuffer;
            dprintf("[ArkTools_Ring0] IOCTRL_MEMWRITE addr read:%p, PID=%d, Length=%d, Content=%p\r\n",
                pMem->nAddress, pMem->nPID, pMem->nLength, &pMem->pContent);

            //柔性数组取地址当参数.这样操作的就是柔性数组的空间.而该空间位于返回的数据buffer中.
            status = MyReadProcessMemory(pMem->nPID, pMem->nAddress, pMem->nLength,
                &pMem->pContent, pMem->nLength, &uByte);

            dprintf("[KViewer] IOCTRL_MEMREAD MyReadProcessMemory buffer:%p\r\n", pMem->pContent);
            //DbgBreakPoint();
            if (NT_SUCCESS(status))
            {
                //返回的大小要加上结构体的大小.因为是柔性数组.数据在结构体后面.
                dprintf("[ArkTools_Ring0] IOCTRL_MEMREAD bytes read:%d\r\n", uByte);
                uOutSize = sizeof(struct tagMemContent) + uByte;// sizeof(struct tagMemContent*);
            }
            else
            {
                dprintf("[ArkTools_Ring0] IOCTRL_MEMREAD Error file:%s line:%d\n",
                    __FILE__, __LINE__);
                uOutSize = 0;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            dprintf("[ArkTools_Ring0] DispatchDeviceControl __except file:%s line:%d\n",
                __FILE__, __LINE__);
        }
    }
    break;
    //
    // TODO: Add execute code here.
    //

    case IOCTRL_GET_ETHREAD:
    {
        pEPROCESSAddr = *(long long*)pIoBuffer;
        dprintf("[ArkTools] IOCTRL_GET_ETHREAD:%p\r\n", pEPROCESSAddr);

        __try
        {
            //Win32StartAddress 0x228
            //0X2F8

            // ThreadListHead   [Type: _LIST_ENTRY]  EPROCESS+0X50的位置是线程链表头
            PLIST_ENTRY  ListEntry = (PLIST_ENTRY)((char*)pEPROCESSAddr + 0X50);

            if (ListEntry && MmIsAddressValid(ListEntry) && MmIsAddressValid(ListEntry->Flink))
            {
                UINT_PTR    MaxCount = PAGE_SIZE * 2;
                PLIST_ENTRY TravelList = ListEntry->Flink;
                PTHREADINFO pThreadInfo = (PTHREADINFO)pIoBuffer;
                while (MmIsAddressValid(TravelList) && TravelList != ListEntry && MaxCount--)
                {
                    //TravelList的位置是线程链表.   减0x1b0的位置是ETHREAD
                    if (MmIsAddressValid((PUINT8)TravelList - 0x1B0))
                    {
                        PETHREAD EThread = (PETHREAD)((PUINT8)TravelList - 0x1B0);
                      
                        pThreadInfo[i].EThread = EThread;
                        pThreadInfo[i].Win32StartAddr = *(PUINT32)((char*)EThread + 0x228);
                        pThreadInfo[i].TEB = *(PUINT32)((char*)EThread + 0x20);
                        pThreadInfo[i].Priority = *((char*)EThread + 0x33);
                        pThreadInfo[i].ContextSwitch = *(unsigned long*)((char*)EThread + 0x4c);
                        pThreadInfo[i].ThreadID = *(PUINT32)((char*)EThread + 0x1ec + sizeof(void*));
                        pThreadInfo[i].Status = *((char*)EThread + 0x2d);
                        dprintf("[ArkTools] IOCTRL_GET_ETHREAD_EThread:%p\r\n", EThread);
                        //dprintf("[ArkTools] IOCTRL_GET_ETHREAD_Win32StartAddress:%p\r\n", *(PUINT32)((char*)EThread + 0x228));
                        //dprintf("[ArkTools] IOCTRL_GET_ETHREAD_TEB:%p\r\n", *(PUINT32)((char*)EThread + 0x20));
                        
                        //dprintf("[ArkTools] IOCTRL_GET_ETHREAD_Priority:%d\r\n", *((char*)EThread + 0x33));
                        //dprintf("[ArkTools] IOCTRL_GET_ETHREAD_ContextSwitches:%d\r\n", *(unsigned long*)((char*)EThread + 0x4c));
                        dprintf("[ArkTools] IOCTRL_GET_ETHREAD_TID:%d\r\n", *(PUINT32)((char*)EThread + 0x1ec + sizeof(void*)));

                        TravelList = TravelList->Flink;
                        i++;
                    }
                }
                dprintf("[ArkTools] ETHREAD_COUNT:%d\r\n", i);

                if (i * sizeof(THREADINFO) < uOutSize)//如果线程的数据大于outsize.仅返回outsize大小
                {
                    uOutSize = i * sizeof(THREADINFO);
                }

                status = STATUS_SUCCESS;
            }
            else
            {
                dprintf("[ArkTools_Ring0] IOCTRL_GET_ETHREAD Error: MmIsAddressValid(false)!\r\n");
                uOutSize = 0;
                status = STATUS_UNSUCCESSFUL;
            }
            
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            uOutSize = 0;
            dprintf("[ArkTools_Ring0] IOCTRL_GET_EPROCESS Error: File:%s,Line:%d!\r\n", __FILE__, __LINE__);
            status = STATUS_UNSUCCESSFUL;
        }

    }
    break;

   
    case IOCTRL_GET_KEYBOARD_MAJOR_FUNC:
    {
        status = GetKeyBoardMajorFunc(pIoBuffer,&uOutSize);
    }
    break;

    case IOCTRL_GET_MOUSE_MAJOR_FUNC:
    {
        status = GetMouseMajorFunc(pIoBuffer,&uOutSize);
    }
    break;

    case IOCTRL_GET_I8042PRT_MAJOR_FUNC:
    {
        status = GetI8042prtMajorFunc(pIoBuffer,&uOutSize);
    }
    break;

    case IOCTRL_GET_TCPIP_MAJOR_FUNC:
    {
        status = GetTCIPMajorFunc(pIoBuffer,&uOutSize);
    }
    break;

    default:
    {
        // Invalid code sent
        dprintf("[ArkTools] Unknown IOCTL: 0x%X (%04X,%04X)\r\n",
            uIoControlCode,
            DEVICE_TYPE_FROM_CTL_CODE(uIoControlCode),
            IoGetFunctionCodeFromCtlCode(uIoControlCode));
        status = STATUS_INVALID_PARAMETER;
    }
    break;
    }

    if (status == STATUS_SUCCESS)
    {
        pIrp->IoStatus.Information = uOutSize;
    }
    else
    {
        pIrp->IoStatus.Information = 0;
    }

    // Complete the I/O Request
    pIrp->IoStatus.Status = status;

    IoCompleteRequest(pIrp, IO_NO_INCREMENT);

    return status;
}
/* EOF */
