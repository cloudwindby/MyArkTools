#ifndef ARKTOOLS_H
#define ARKTOOLS_H

#include <QMainWindow>
#include <qDebug>
#include <QDir>
#include <QMenu>
#include <QMessageBox>
#include "driverload.h"
#include "readmemdialog.h"
#include "threaddialog.h"
#include "../common/common.h"

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"strsafe.lib")

namespace Ui {
class ArkTools;
}

class ArkTools : public QMainWindow
{
    Q_OBJECT

public:
    explicit ArkTools(QWidget *parent = 0);
    ~ArkTools();

    //驱动相关
    //------------------------------------

    //驱动加载
    int m_LoadDriver();

    //退出事件(卸载驱动)
    virtual void closeEvent(QCloseEvent *event);

    //初始化设备
    //int m_InitDevice();



    //全局描述符表相关(GDT)
    //------------------------------------

    //初始化表
    bool m_InitGdtTable();

    //获取系统信息(逻辑处理器个数)
    int m_GetSystemInfo();

    //显示GDT数据
    int m_ShowGDT();


    //中断描述符表相关(IDT)
    //-------------------------------------

    //初始化表
    bool m_InitIDTTable();

    //显示IDT数据
    int m_ShowIDT();

    //进程相关
    //-------------------------------------

    //初始化表
    bool m_InitProcessTable();

    //获取进程信息
    int m_GetProcessInfo();

    //获取文件厂商
    QString m_GetCompanyName(QString);

    //获取进程EPROCESS
    QString m_GetEPROCESS();

    //进程路径转换 like \\Device\\HarddiskVolume1\xxx.sys to   c:\xxx.sys
    BOOLEAN m_NtPathToDosPathW(WCHAR *, WCHAR *);

    //获取PID(进程右键相关)
    DWORD GetPID();

    //获取文件名(进程右键相关)
    QString GetProcessName();


    //系统服务描述符表(SSDT)
    //------------------------------------

    //初始化表
    bool m_InitSSDTTable();

    //获取SSDT个数
    int m_GetSSDTCount();

    //获取SSDT数据
    int m_GetSSDTData(DWORD);

    //ShadowSSDT (SSSDT)
    //------------------------------------

    //初始化表
    bool m_InitSSSDTTable();

    //获取SSSDT个数
    int m_GetShadowSSDTCount();

    //获取SSSDT数据
    int m_GetShadowSSDTData(DWORD);

    //majorFunc
    //键盘
    bool m_InitKeyBoardMajorFuncTable(PVOID);
    int m_GetKeyBoardMajorFuncData(PVOID,DWORD IOCTRL_CODE);

//    //鼠标
//    bool m_InitMouseMajorFuncTable();
//    int m_GetMouseMajorFuncData();

//    //8042
//    bool m_InitI8042prtMajorFuncTable();
//    int m_GetI8042prtMajorFuncData();

//    //TCP/IP
//    bool m_InitTcpipMajorFuncTable();
//    int m_GetTcpipMajorFuncData();

private slots:

    //进程右键菜单处理
    //-----------------------------------

    //读内存数据
    void slot_readmem();

    //结束进程
    void slot_terminate();

    //强制结束进程
    void slot_terminatesystem();

    //刷新
    void slot_refresh();

    //查看进程线程
    void slot_thread();

    void on_EPROCESS_Table_customContextMenuRequested(const QPoint &pos);
    void on_EPROCESS_Widget_currentChanged(int index);
    void on_KERNEL_TabWidget_currentChanged(int index);

private:
    Ui::ArkTools *ui;

    //进程右键菜单
    QSharedPointer<QMenu> m_menu;

    //系统信息(主要用来获取逻辑处理器核心数)
    SYSTEM_INFO m_si;

    //当前目录(用来获取驱动的路径)
    QString m_CurrentDir;

    //设备对象的文件句柄(用来与驱动通信)
    HANDLE m_hFile;

    //处理GDT数据的数组
    char m_szGDT[1024];

    //读取进程内存的Dialog
    QSharedPointer<ReadMemDialog> m_ReadmemDialog;

    //线程对话框
    QSharedPointer<ThreadDialog> m_ThreadDialog;

    //驱动加载类
    DriverLoad m_DriverLoad;
};

#endif // ARKTOOLS_H
