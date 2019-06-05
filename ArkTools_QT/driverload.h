#ifndef DRIVERLOAD_H
#define DRIVERLOAD_H

#include <windows.h>

#pragma comment(lib,"User32.lib")


//DriverLoad Error Code

//InitDevice
#define CREATE_DEVICE_ERROR 0
#define CREATE_DEVICE_SUCCESS 1

//LoadDriver
#define OPENSCMANAGER_ERROR 10
#define CREATESERVICE_ERROR 11
#define CREATESERVICE_OK 12

//StartDriver
#define STARTSERVICE_ERROR 20
#define STARTSERVICE_OK 21

//StopDriver
#define STOPSERVICE_OK 30
#define STOPSERVICE_ERROR 31

//DeleteDriver
#define DELETEDRIVER_OK 40

//驱动加载
class DriverLoad
{
public:
    DriverLoad();
    ~DriverLoad();

    //初始化设备信息
    int InitDevice(LPCTSTR lpLinkName);
    //加载驱动
    int LoadDriver(LPCTSTR lpServiceName,LPCTSTR lpDisplayName,LPCTSTR lpBinaryPathName);
    //启动驱动
    int StartDriver();
    //控制驱动
    int StopDriver();
    //卸载驱动
    int DeleteDriver();

    HANDLE hFile() const;
    void setHFile(const HANDLE &hFile);

private:
    //文件句柄
    HANDLE m_hFile = NULL;
    //服务管理器句柄
    SC_HANDLE m_hSCM = NULL;
    //服务句柄
    SC_HANDLE m_hService = NULL;
};

#endif // DRIVERLOAD_H
