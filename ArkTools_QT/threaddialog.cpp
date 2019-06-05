#include "threaddialog.h"
#include "ui_threaddialog.h"
#include "../common/common.h"
#include <qDebug>

char g_ThreadState[10][24] =
{
"预置",
"就绪",
"运行",
"备用",
"终止",
"等待",
"过度",
"延迟就绪",
"门等待",
"未知",
};

ThreadDialog::ThreadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ThreadDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
}

ThreadDialog::~ThreadDialog()
{
    delete ui;
}

bool ThreadDialog::m_InitThreadTable()
{
    QStringList labels;
    labels << QStringLiteral("线程ID")
           << QStringLiteral("ETHREAD")
           << QStringLiteral("Teb")
           << QStringLiteral("优先级")
           << QStringLiteral("线程入口")
           << QStringLiteral("模块")
           << QStringLiteral("切换次数")
           << QStringLiteral("线程状态");

    ui->THREAD_Table->setColumnCount(8);
    ui->THREAD_Table->setRowCount(0);
    ui->THREAD_Table->verticalHeader()->setDefaultSectionSize(20);
    ui->THREAD_Table->setColumnWidth(0, 50);
    ui->THREAD_Table->setColumnWidth(1, 70);
    ui->THREAD_Table->setColumnWidth(2, 70);
    ui->THREAD_Table->setColumnWidth(3, 50);
    ui->THREAD_Table->setColumnWidth(4, 70);
    ui->THREAD_Table->setColumnWidth(6, 60);
    ui->THREAD_Table->setHorizontalHeaderLabels(labels);
    ui->THREAD_Table->setSortingEnabled(true);
    ui->THREAD_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->THREAD_Table->setSelectionBehavior(QAbstractItemView::SelectRows);

    return true;
}

void ThreadDialog::showEvent(QShowEvent *event)
{
    m_InitThreadTable();
    setWindowTitle("["+m_ProcessName+QStringLiteral("]进程线程"));
    GetThreadData();
}

void ThreadDialog::setTitle(const QString &ProcessName)
{
    m_ProcessName = ProcessName;
}

int ThreadDialog::GetThreadData()
{

    PTHREADINFO pThreadInfo = new THREADINFO[100];//写死了.注意应先获取线程个数.动态申请内存.
    int nRowCont = 0;

    if(pThreadInfo != nullptr)
    {
        DWORD bytesOfRead;
        DeviceIoControl(m_hFile, IOCTRL_GET_ETHREAD, &m_EPROCESS, sizeof(long long), pThreadInfo,sizeof(THREADINFO) * 100, &bytesOfRead, NULL);
         qDebug() << "[ArkTools_Ring3]DeviceIoControl:bytesOfRead" << bytesOfRead;
        if(bytesOfRead != 0)
        {
            for(int i = 0;i < bytesOfRead / sizeof(THREADINFO);i++)
            {
                nRowCont = ui->THREAD_Table->rowCount();
                ui->THREAD_Table->insertRow(nRowCont);//增加一行

                ui->THREAD_Table->setItem(nRowCont,0,new QTableWidgetItem(QString::number(pThreadInfo[i].ThreadID)));
                ui->THREAD_Table->setItem(nRowCont,1,new QTableWidgetItem("0x" + tr("%1").arg(pThreadInfo[i].EThread,8,16,QLatin1Char('0')).toUpper()));
                ui->THREAD_Table->setItem(nRowCont,2,new QTableWidgetItem("0x" + tr("%1").arg(pThreadInfo[i].TEB,8,16,QLatin1Char('0')).toUpper()));
                ui->THREAD_Table->setItem(nRowCont,3,new QTableWidgetItem(QString::number(pThreadInfo[i].Priority)));
                ui->THREAD_Table->setItem(nRowCont,4,new QTableWidgetItem("0x" + tr("%1").arg(pThreadInfo[i].Win32StartAddr,8,16,QLatin1Char('0')).toUpper()));
                ui->THREAD_Table->setItem(nRowCont,6,new QTableWidgetItem(QString::number(pThreadInfo[i].ContextSwitch)));
                ui->THREAD_Table->setItem(nRowCont,7,new QTableWidgetItem(QString::fromLocal8Bit(g_ThreadState[pThreadInfo[i].Status])));
                qDebug() << "[ArkTools_Ring3]TEB:" << pThreadInfo[i].TEB;
                qDebug() << "[ArkTools_Ring3]ContextSwitch:" << pThreadInfo[i].ContextSwitch;
                qDebug() << "[ArkTools_Ring3]Priority:"  << pThreadInfo[i].Priority;
                qDebug() << "[ArkTools_Ring3]ThreadID:"  << pThreadInfo[i].ThreadID;
            }
        }

    }
    else
    {
        qDebug() << "[ArkTools_Ring3]alloc memory failure";
    }



    if(pThreadInfo != nullptr)
    {
        delete[] pThreadInfo;
    }

    return 0;


}

long long ThreadDialog::EPROCESS() const
{
    return m_EPROCESS;
}

void ThreadDialog::setEPROCESS(const long long &EPROCESS)
{
    m_EPROCESS = EPROCESS;
}

HANDLE ThreadDialog::hFile() const
{
    return m_hFile;
}

void ThreadDialog::setHFile(const HANDLE &hFile)
{
    m_hFile = hFile;
}
