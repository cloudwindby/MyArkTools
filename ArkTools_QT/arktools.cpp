#include "arktools.h"
#include "ui_arktools.h"


ArkTools::ArkTools(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArkTools)
{
    ui->setupUi(this);

    //初始化各种表
    m_InitProcessTable();

    //进程右键菜单
    m_menu = QSharedPointer<QMenu>(new QMenu());

    //获取系统信息(主要获取逻辑处理器核心数)
    m_GetSystemInfo();

    //设置字体
    QFont font("Microsoft YaHei",10,50); //第一个属性是字体（微软雅黑），第二个是大小，第三个是粗细
    ui->GDT_Label->setFont(font);
    //显示
    ui->GDT_Label->setText(QStringLiteral("逻辑处理器:")+QString::number(m_si.dwNumberOfProcessors));

    //获取当前路径
    m_CurrentDir = QDir::currentPath();
    //加载驱动
    DWORD dwStatus = m_LoadDriver();
    if(dwStatus == LOADDRIVER_OK)
    {
        QAction* pTerminate = m_menu->addAction(QStringLiteral("结束进程"));
        QAction* pTerminateSystem = m_menu->addAction(QStringLiteral("强制结束进程"));
        QAction* pRefresh = m_menu->addAction(QStringLiteral("刷新"));
        QAction* pThread = m_menu->addAction(QStringLiteral("查看进程线程"));
        QAction* pReadMem = m_menu->addAction(QStringLiteral("读内存数据"));
        connect(pReadMem,SIGNAL(triggered()),this,SLOT(slot_readmem()));
        connect(pTerminateSystem,SIGNAL(triggered()),this,SLOT(slot_terminatesystem()));
        connect(pTerminate,SIGNAL(triggered()),this,SLOT(slot_terminate()));
        connect(pRefresh,SIGNAL(triggered()),this,SLOT(slot_refresh()));
        connect(pThread,SIGNAL(triggered()),this,SLOT(slot_thread()));
        qDebug() << "[ArkTools_Ring3] CREATE_DEVICE_SUCCESS";

        //驱动加载成功开始处理
        m_GetProcessInfo();
    }
}

ArkTools::~ArkTools()
{
    delete ui;
}

void ArkTools::closeEvent(QCloseEvent *event)
{
    if(m_DriverLoad.StopDriver() == STOPSERVICE_OK)
    {
        m_DriverLoad.DeleteDriver();
    }
}

int ArkTools::m_GetSystemInfo()
{
    GetNativeSystemInfo(&m_si);
    return 0;
}

int ArkTools::m_LoadDriver()
{

    DWORD dwStatus = 0;
    m_CurrentDir += LOCAL_SYS_FILE_NAME;//m_CurrentDir += "/arktools.sys";加载的驱动文件在同级目录下
    m_CurrentDir.replace("/","\\");
    //qDebug("[ArkTools_Ring3]:FilePath:%ls",m_CurrentDir.toStdWString().c_str());
    dwStatus = m_DriverLoad.LoadDriver(SERVICE_NAME,DISPLAY_NAME,m_CurrentDir.toStdWString().c_str());
    if(dwStatus == CREATESERVICE_OK)
    {
        dwStatus = m_DriverLoad.StartDriver();
        if(dwStatus == STARTSERVICE_OK)
        {
            dwStatus = m_DriverLoad.InitDevice(WIN32_LINK_NAME);
            if(dwStatus == CREATE_DEVICE_SUCCESS)
            {
                m_hFile = m_DriverLoad.hFile();
                return LOADDRIVER_OK;
            }
        }
    }

    return LOADDRIVER_ERROR;
}

void ArkTools::on_EPROCESS_Table_customContextMenuRequested(const QPoint &pos)
{
    if(ui->EPROCESS_Table->rowCount() < 1)
    {
        return;
    }
    else if(ui->EPROCESS_Table->itemAt(pos) != 0)
    {
        m_menu->exec(QCursor::pos());
    }
}

void ArkTools::on_EPROCESS_Widget_currentChanged(int index)
{
    qDebug("[ArkTools_Ring3]Tab:%d",index);
    if(index == 1)
    {
        ui->SSDT_Table->clear();
        m_InitSSDTTable();
        m_GetSSDTCount();
    }
    if(index == 0)
    {
        ui->EPROCESS_Table->clear();
        m_InitProcessTable();
        m_GetProcessInfo();
    }
    if(index == 2)
    {

    }
    if(index == 3)
    {
//        ui->TCPIP_Table->clear();
//        m_InitTcpipMajorFuncTable();
//        m_GetTcpipMajorFuncData();
        ui->TCPIP_Table->clear();
        m_InitKeyBoardMajorFuncTable(ui->TCPIP_Table);
        m_GetKeyBoardMajorFuncData(ui->TCPIP_Table,IOCTRL_GET_TCPIP_MAJOR_FUNC);
    }

}

void ArkTools::on_KERNEL_TabWidget_currentChanged(int index)
{
    qDebug("[ArkTools_Ring3]Tab:%d",index);
    if(index == 0)
    {
        ui->SSDT_Table->clear();
        m_InitSSDTTable();
        m_GetSSDTCount();
    }
    if(index == 1)
    {
        ui->SSSDT_Table->clear();
        m_InitSSSDTTable();
        m_GetShadowSSDTCount();
    }
    if(index == 2)
    {
        ui->GDT_Table->clear();
        m_InitGdtTable();
        m_ShowGDT();
    }
    if(index == 3)
    {
        ui->IDT_Table->clear();
        m_InitIDTTable();
        m_ShowIDT();
    }
    if(index == 4)
    {
        ui->KEYBOARD_Table->clear();
        m_InitKeyBoardMajorFuncTable(ui->KEYBOARD_Table);
        m_GetKeyBoardMajorFuncData(ui->KEYBOARD_Table,IOCTRL_GET_KEYBOARD_MAJOR_FUNC);
    }
    if(index == 5)
    {
//        ui->I8042PRT_Table->clear();
//        m_InitI8042prtMajorFuncTable();
//        m_GetI8042prtMajorFuncData();
        ui->I8042PRT_Table->clear();
        m_InitKeyBoardMajorFuncTable(ui->I8042PRT_Table);
        m_GetKeyBoardMajorFuncData(ui->I8042PRT_Table,IOCTRL_GET_I8042PRT_MAJOR_FUNC);
    }
    if(index == 6)
    {
//        ui->MOUSE_Table->clear();
//        m_InitMouseMajorFuncTable();
//        m_GetMouseMajorFuncData();
        ui->MOUSE_Table->clear();
        m_InitKeyBoardMajorFuncTable(ui->MOUSE_Table);
        m_GetKeyBoardMajorFuncData(ui->MOUSE_Table,IOCTRL_GET_MOUSE_MAJOR_FUNC);
    }

}
