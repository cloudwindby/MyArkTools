#include "ArkTools.h"

NTSTATUS MyGetDirBase(ULONG pid, PVOID * pDirBase)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    char* chProcess = NULL;
    char* chEndProcess = NULL;
    ULONG uCurPID = 0;

    chProcess = PsGetCurrentProcess();
    chEndProcess = chProcess;
    do
    {
        uCurPID = *(ULONG*)(chProcess + 0x84);

        if (uCurPID == pid)
        {
            *pDirBase = *(PVOID*)(chProcess + 0x18);	// 获取页基址
            dprintf("[ArkTools] MyGetDirBase pDirBase=%p\n", pDirBase);
            return STATUS_SUCCESS;
        }

        chProcess = (char*)(*(int*)(chProcess + 0x88) - 0x88);

    } while (chProcess != chEndProcess);

    return status;
}

NTSTATUS MyReadProcessMemory(ULONG pid, PVOID address, ULONG length, PVOID buffer, ULONG size, ULONG * lpNumberOfBytesRead)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    PVOID pDirBase = NULL;
    PVOID pOldDirBase = NULL;

    __try
    {
        if (length <= 0 || size <= 0 || !MmIsAddressValid(buffer)
            || !(MmIsAddressValid(lpNumberOfBytesRead)))
        {
            dprintf("[ArkTools] MyReadProcessMemory Invalid Param file:%s line:%d\n",
                __FILE__, __LINE__);
            return STATUS_INVALID_PARAMETER;
        }
        dprintf("[ArkTools] MyReadProcessMemory Buffer Address=%x", buffer);

        // 获取页目录地址
        if (!NT_SUCCESS(MyGetDirBase(pid, &pDirBase)))
        {
            dprintf("[ArkTools] MyGetDirBase Error file:%s line:%d\n",
                __FILE__, __LINE__);
            return status;
        }

        // 切换CR3
        __asm
        {
            pushad
                pushf
                mov eax, cr3
                mov pOldDirBase, eax
                mov eax, pDirBase
                mov cr3, eax
                popf
                popad
        }

        if (!MmIsAddressValid(buffer))
        {
            dprintf("[ArkTools] MyReadProcessMemory Invalid Address=%x file:%s line:%d\n",
                buffer, __FILE__, __LINE__);
            status = STATUS_INVALID_ADDRESS;
        }
        if (!MmIsAddressValid(address))
        {
            dprintf("[ArkTools] MyReadProcessMemory Invalid Address=%x file:%s line:%d\n",
                address, __FILE__, __LINE__);
            status = STATUS_INVALID_ADDRESS;
        }
        else
        {
            length = length > size ? size : length;
            RtlCopyMemory(buffer, address, length);
            //DbgBreakPoint();
            *lpNumberOfBytesRead = length;
            dprintf("[ArkTools] MyReadProcessMemory buffer=%p", buffer);
            dprintf("[ArkTools] MyReadProcessMemory read bytes=%d", length);
        }

        // 再次切换CR3
        __asm
        {
            pushad
                pushf
                mov eax, pOldDirBase
                mov cr3, eax
                popf
                popad
        }
        status = STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        dprintf("[ArkTools] MyReadProcessMemory __except file:%s line:%d\n",
            __FILE__, __LINE__);
    }

    return status;
}