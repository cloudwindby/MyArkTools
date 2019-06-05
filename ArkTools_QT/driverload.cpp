#include "driverload.h"

DriverLoad::DriverLoad()
{
}

DriverLoad::~DriverLoad()
{

}

int DriverLoad::LoadDriver(LPCTSTR lpServiceName,LPCTSTR lpDisplayName,LPCTSTR lpBinaryPathName)
{
    m_hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(m_hSCM == NULL)
    {
        DWORD dwError  = GetLastError();
        if(dwError == ERROR_ACCESS_DENIED)
        {

            OutputDebugString(L"[DriverLoad]LoadDriver_ERROR_ACCESS_DENIED!\n");
        }
        return OPENSCMANAGER_ERROR;
    }

    OutputDebugString(L"[DriverLoad]lpServiceName:");
    OutputDebugString(lpServiceName);
    OutputDebugString(L"[DriverLoad]lpDisplayName:");
    OutputDebugString(lpDisplayName);
    OutputDebugString(L"[DriverLoad]lpBinaryPathName:");
    OutputDebugString(lpBinaryPathName);
    m_hService = CreateService(m_hSCM,
                               lpServiceName,
                               lpDisplayName,
                               SERVICE_START|SERVICE_STOP| DELETE|SERVICE_QUERY_STATUS,
                               SERVICE_KERNEL_DRIVER,
                               SERVICE_DEMAND_START,
                               SERVICE_ERROR_NORMAL,
                               //m_CurrentDir.toStdWString().c_str(),
                               lpBinaryPathName,
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               NULL);
    if (m_hService == NULL)
    {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_SERVICE_EXISTS)
        {
            m_hService = OpenService(m_hSCM, lpServiceName, SERVICE_START |SERVICE_STOP| DELETE|SERVICE_QUERY_STATUS);
            DWORD dwOpenServiceError = GetLastError();
            if (dwOpenServiceError == ERROR_ACCESS_DENIED)
            {
                OutputDebugString(L"[DriverLoad]LoadDriver_拒绝访问!\n");
                return CREATESERVICE_ERROR;
            }
            else if(dwOpenServiceError == ERROR_INVALID_HANDLE)
            {
                OutputDebugString(L"[DriverLoad]LoadDriver_句柄无效!\n");
                return CREATESERVICE_ERROR;
            }
            else if(dwOpenServiceError == ERROR_INVALID_NAME)
            {
                OutputDebugString(L"[DriverLoad]LoadDriver_服务名无效!\n");
                return CREATESERVICE_ERROR;
            }
            else if(dwOpenServiceError == ERROR_SERVICE_DOES_NOT_EXIST)
            {
                OutputDebugString(L"[DriverLoad]LoadDriver_服务不存在!\n");
                return CREATESERVICE_ERROR;
            }
            else if(dwOpenServiceError == ERROR_SERVICE_EXISTS)
            {
                TCHAR msgbuf[256];
                wsprintf(msgbuf, L"[DriverLoad]LoadDriver_SERVICE_EXISTS:m_hService_handle:%p\n",m_hService);
                OutputDebugString(msgbuf);
                return CREATESERVICE_OK;
            }
            else
            {
                TCHAR msgbuf[256];
                wsprintf(msgbuf, L"[DriverLoad]LoadDriver_OpenService_Other Error:%d:%p\n",dwOpenServiceError);
                OutputDebugString(msgbuf);
                return CREATESERVICE_OK;
            }

        }//end if (dwError == ERROR_SERVICE_EXISTS)
        else if(dwError == ERROR_INVALID_HANDLE)
        {
            OutputDebugString(L"[DriverLoad]LoadDriver_句柄无效!\n");
            return CREATESERVICE_ERROR;
        }
        else if(dwError == ERROR_INVALID_NAME)
        {
            OutputDebugString(L"[DriverLoad]LoadDriver_服务名无效!\n");
            return CREATESERVICE_ERROR;
        }
        else if(dwError == ERROR_FILE_NOT_FOUND)
        {
            OutputDebugString(L"[DriverLoad]LoadDriver_文件未找到!\n");
            return CREATESERVICE_ERROR;
        }
        else
        {
            TCHAR msgbuf[256];
            wsprintf(msgbuf, L"[DriverLoad]CreateService_Other Error:%d\n",dwError);
            OutputDebugString(msgbuf);
            return CREATESERVICE_ERROR;
        }

    }//end if (m_hService == NULL)
    else
    {
        OutputDebugString(L"[DriverLoad]LoadDriver_CREATESERVICE_OK!\n");
        return CREATESERVICE_OK;
    }
}

int DriverLoad::StartDriver()
{
    BOOL status = StartService(m_hService, 0, NULL);
    if(status == FALSE)
    {
        DWORD dwError = GetLastError();
        switch(dwError)
        {
        case ERROR_ACCESS_DENIED:
        {
            OutputDebugString(L"[DriverLoad]StartDriver_句柄没有SERVICE_START访问权限!\n");
            return STARTSERVICE_ERROR;
        }
        case ERROR_INVALID_HANDLE:
        {
            OutputDebugString(L"[DriverLoad]StartDriver_启动服务的句柄无效!\n");
            return STARTSERVICE_ERROR;
        }
        case ERROR_PATH_NOT_FOUND:
        {
            OutputDebugString(L"[DriverLoad]StartDriver_无法找到服务二进制文件!\n");
            return STARTSERVICE_ERROR;
        }
        case ERROR_SERVICE_ALREADY_RUNNING:
        {
            OutputDebugString(L"[DriverLoad]StartDriver_服务的实例已经在运行!\n");
            return STARTSERVICE_ERROR;
        }
        case ERROR_SERVICE_DISABLED:
        {
            OutputDebugString(L"[DriverLoad]StartDriver_服务已被禁用!\n");
            return STARTSERVICE_ERROR;
        }
        case ERROR_SERVICE_MARKED_FOR_DELETE:
        {
            OutputDebugString(L"[DriverLoad]StartDriver_该服务已被标记为删除!\n");
            return STARTSERVICE_ERROR;
        }
        case ERROR_SERVICE_NO_THREAD:
        {
            OutputDebugString(L"[DriverLoad]StartDriver_无法为服务创建线程!\n");
            return STARTSERVICE_ERROR;
        }
        default:
        {
            TCHAR msgbuf[256];
            wsprintf(msgbuf, L"[DriverLoad]StartDriver_OtherError:%d\n",dwError);
            OutputDebugString(msgbuf);
            return STARTSERVICE_ERROR;
        }
        }//end switch(dwError)
    }
    else
    {
        return STARTSERVICE_OK;
    }
}

int DriverLoad::InitDevice(LPCTSTR lpLinkName)
{
    TCHAR msgbuf[256];
    wsprintf(msgbuf, L"[DriverLoad]lpLinkName:%s\n",lpLinkName);
    OutputDebugString(msgbuf);
    m_hFile = CreateFile(lpLinkName,
                         GENERIC_READ | GENERIC_WRITE,
                         NULL,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL);


    if (m_hFile == NULL)
    {
        wsprintf(msgbuf, L"[DriverLoad]InitDevice_GetLastError:%d\n",GetLastError());
        OutputDebugString(msgbuf);
        return CREATE_DEVICE_ERROR;
    }
    return CREATE_DEVICE_SUCCESS;
}

int DriverLoad::StopDriver()
{
    SERVICE_STATUS ss;
	DWORD dwError = 0;
    if(m_hFile != NULL)
    {
        CloseHandle(m_hFile);
    }
	ControlService(m_hService, SERVICE_CONTROL_STOP, &ss);
	dwError = GetLastError();
	if(dwError == ERROR_SERVICE_CANNOT_ACCEPT_CTRL)
	{
		 while(ss.dwCurrentState != SERVICE_STOPPED )
		{
			Sleep( ss.dwWaitHint );
		}
        OutputDebugString(L"[DriverLoad]StopDriver OK!\n");
		return STOPSERVICE_OK;
	}
	else
	{
        TCHAR msgbuf[256];
        wsprintf(msgbuf, L"[DriverLoad]StopDriver_Other Error:%d\n",dwError);
        OutputDebugString(msgbuf);
		return STOPSERVICE_ERROR;
	}
}

int DriverLoad::DeleteDriver()
{
	if(m_hFile != NULL)
    {
        CloseHandle(m_hFile);
    }
    if(m_hService != NULL)
    {
        DeleteService(m_hService);
        OutputDebugString(L"[DriverLoad]DeleteService!\n");
        CloseHandle(m_hService);
    }
    if(m_hSCM != NULL)
    {
        CloseServiceHandle(m_hSCM);
    }
    return DELETEDRIVER_OK;
}

HANDLE DriverLoad::hFile() const
{
    return m_hFile;
}

void DriverLoad::setHFile(const HANDLE &hFile)
{
    m_hFile = hFile;
}



