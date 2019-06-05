#include "arktools.h"
#include "ui_arktools.h"
#include "idt.h"

bool ArkTools::m_InitIDTTable()
{
    QStringList labels;
    labels << QStringLiteral("CPU序号")
           << QStringLiteral("序号")
           << QStringLiteral("函数名")
           << QStringLiteral("段选择子(Index)")
           << QStringLiteral("当前函数地址")
           << QStringLiteral("HOOK")
           << QStringLiteral("原始函数地址")
           << QStringLiteral("当前函数地址所在模块");

    ui->IDT_Table->setColumnCount(8);
    ui->IDT_Table->setRowCount(0);
    ui->IDT_Table->verticalHeader()->setDefaultSectionSize(15);
    ui->IDT_Table->setColumnWidth(0, 60);
    ui->IDT_Table->setColumnWidth(1, 60);
    ui->IDT_Table->setColumnWidth(2, 165);
    ui->IDT_Table->setColumnWidth(3, 100);
    ui->IDT_Table->setColumnWidth(4, 80);
    //ui->IDT_Table->setColumnWidth(0, 120);
    //ui->IDT_Table->setColumnWidth(0, 120);
    ui->IDT_Table->setHorizontalHeaderLabels(labels);
    ui->IDT_Table->setSortingEnabled(true);
    ui->IDT_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->IDT_Table->setSelectionBehavior(QAbstractItemView::SelectRows);

    return true;
}

int ArkTools::m_ShowIDT()
{
    int nCurrentProcessor;
    char szIDT[2048] = { 0 };

    for(int j = 0;j < m_si.dwNumberOfProcessors;j++)
    {
        nCurrentProcessor = j;
        DWORD bytesOfRead = 0;
        DeviceIoControl(m_hFile, IOCTL_GET_IDT, &nCurrentProcessor, sizeof(nCurrentProcessor), szIDT,2048, &bytesOfRead, NULL);

        if(bytesOfRead == 0)
        {
            continue;
        }
        PIDTENTRY p = (PIDTENTRY)szIDT;
        for(int i = 0;i < 256;i++)
        {

            int nRowCont;
            nRowCont=ui->IDT_Table->rowCount();
            ui->IDT_Table->insertRow(nRowCont);//增加一行

            ui->IDT_Table->setItem(nRowCont,0,new QTableWidgetItem(QString::number(j)));


            ui->IDT_Table->setItem(nRowCont,1,new QTableWidgetItem("0x" + tr("%1").arg(i,2,16,QLatin1Char('0')).toUpper()));
            ui->IDT_Table->setItem(nRowCont,2,new QTableWidgetItem(QString::fromLocal8Bit(g_IDT[i])));
            unsigned int SelectorIndex =
                    (p+i)->Selector >> 3;
            ui->IDT_Table->setItem(nRowCont,3,new QTableWidgetItem("0x" + tr("%1").arg(SelectorIndex,4,16,QLatin1Char('0')).toUpper()));

            unsigned int FuncAddr =
                    ((p+i)->OffsetHi << 16)|(p+i)->OffsetLow;
            ui->IDT_Table->setItem(nRowCont,6,new QTableWidgetItem("0x" + tr("%1").arg(FuncAddr,8,16,QLatin1Char('0')).toUpper()));

        }
        return 0;
    }
}
