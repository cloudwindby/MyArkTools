#include "ArkTools.h"


NTSTATUS GetKeyBoardMajorFunc(IN OUT PVOID IoBuffer, PULONG pSize)
{
        NTSTATUS status;
        UNICODE_STRING ustrDevName;
        PFILE_OBJECT   pFileObj = NULL;
        PDEVICE_OBJECT pDevObj = NULL;
        int i = 0;
        
        RtlInitUnicodeString(&ustrDevName, L"\\Device\\KeyboardClass0");

        status = IoGetDeviceObjectPointer(&ustrDevName,
            FILE_ALL_ACCESS,
            &pFileObj,
            &pDevObj);
        if (NT_SUCCESS(status))
        {
            if (MmIsAddressValid(IoBuffer) && MmIsAddressValid(pSize))
            {
                for (i = 0; i < 28; i++)
                {
                    //dprintf("[ArkTools_Ring0] KeyBoard_DispatchOrdinal=%d DispatchFuncAddr=%p \r\n",i,pDevObj->DriverObject->MajorFunction[i]);
                    ((int*)IoBuffer)[i] = pDevObj->DriverObject->MajorFunction[i];
                }
                *pSize = i * sizeof(PVOID);
                status = STATUS_SUCCESS;
            }
            else
            {
                *pSize = 0;
                status = STATUS_INVALID_ADDRESS;
            }
        }
        dprintf("[ArkTools_Ring0] KeyBoard_pDevObj=%p status=%p\r\n", pDevObj, status);
    
        return status;
}

NTSTATUS GetMouseMajorFunc(IN OUT PVOID IoBuffer, PULONG pSize)
{
    NTSTATUS status;
    UNICODE_STRING ustrDevName;
    PFILE_OBJECT   pFileObj = NULL;
    PDEVICE_OBJECT pDevObj = NULL;
    int i = 0;



    RtlInitUnicodeString(&ustrDevName, L"\\Device\\PointerClass0");

    status = IoGetDeviceObjectPointer(&ustrDevName,
        FILE_ALL_ACCESS,
        &pFileObj,
        &pDevObj);
    if (NT_SUCCESS(status))
    {
        if (MmIsAddressValid(IoBuffer) && MmIsAddressValid(pSize))
        {
            for (i = 0; i < 28; i++)
            {
                //dprintf("[ArkTools_Ring0] Mouse_DispatchOrdinal=%d DispatchFuncAddr=%p \r\n",i,pDevObj->DriverObject->MajorFunction[i]);
                ((int*)IoBuffer)[i] = pDevObj->DriverObject->MajorFunction[i];
            }
            *pSize = i * sizeof(PVOID);
            status = STATUS_SUCCESS;
        }
        else
        {
            *pSize = 0;
            status = STATUS_INVALID_ADDRESS;
        }
    }
    dprintf("[ArkTools_Ring0] Mouse_pDevObj=%p status=%p\r\n", pDevObj, status);

    return status;
}

NTSTATUS GetI8042prtMajorFunc(IN OUT PVOID IoBuffer, PULONG pSize)
{
    NTSTATUS status;
    PFILE_OBJECT   pFileObj = NULL;
    PDEVICE_OBJECT pDevObj = NULL;
    int i = 0;

    UNICODE_STRING DriverName;
    PDRIVER_OBJECT pDriverObj = NULL;
    RtlInitUnicodeString(&DriverName, L"\\Driver\\i8042prt");    // usbhub as an example
    status = ObReferenceObjectByName(
        &DriverName,
        OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
        NULL,
        0,
        *IoDriverObjectType,
        KernelMode,
        NULL,
        (PVOID*)&pDriverObj);

    //GetDeviceName(0x81FB13A8);
    dprintf("[ArkTools_Ring0]GetI8042prtMajorFunc\r\n");

    if (NT_SUCCESS(status))
    {
        if (MmIsAddressValid(IoBuffer) && MmIsAddressValid(pSize))
        {
            for (i = 0; i < 28; i++)
            {
                //dprintf("[ArkTools_Ring0]I8042_DispatchOrdinal=%d DispatchFuncAddr=%p \r\n",i,pDriverObj->MajorFunction[i]);
                ((int*)IoBuffer)[i] = pDriverObj->MajorFunction[i];
            }
            *pSize = i * sizeof(PVOID);
            status = STATUS_SUCCESS;
        }
        else
        {
            *pSize = 0;
            status = STATUS_INVALID_ADDRESS;
        }
        ObDereferenceObject(pDriverObj);
    }
    dprintf("[ArkTools_Ring0] I8042_pDevObj=%p status=%p\r\n", pDevObj, status);

    

    return status;
}

NTSTATUS GetTCIPMajorFunc(IN OUT PVOID IoBuffer, PULONG pSize)
{
    NTSTATUS status;
    UNICODE_STRING ustrDevName;
    PFILE_OBJECT   pFileObj = NULL;
    PDEVICE_OBJECT pDevObj = NULL;
    int i = 0;

    RtlInitUnicodeString(&ustrDevName, L"\\Device\\Tcp");

    status = IoGetDeviceObjectPointer(&ustrDevName,
        FILE_ALL_ACCESS,
        &pFileObj,
        &pDevObj);
    if (NT_SUCCESS(status))
    {
        if (MmIsAddressValid(IoBuffer) && MmIsAddressValid(pSize))
        {
            for (i = 0; i < 28; i++)
            {
                //dprintf("[ArkTools_Ring0] TCIP_DispatchOrdinal=%d DispatchFuncAddr=%p \r\n",i,pDevObj->DriverObject->MajorFunction[i]);
                ((int*)IoBuffer)[i] = pDevObj->DriverObject->MajorFunction[i];
            }
            *pSize = i * sizeof(PVOID);
            status = STATUS_SUCCESS;
        }
        else
        {
            *pSize = 0;
            status = STATUS_INVALID_ADDRESS;
        }
    }
    dprintf("[ArkTools_Ring0] TCIP_pDevObj=%p status=%p\r\n", pDevObj, status);

    return status;
}