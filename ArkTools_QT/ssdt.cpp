#include "arktools.h"
#include "ui_arktools.h"
#include "ssdt.h"

bool ArkTools::m_InitSSDTTable()
{
    QStringList labels;
    labels << QStringLiteral("序号")
           << QStringLiteral("函数名")
           << QStringLiteral("地址")
           << QStringLiteral("模块");

    ui->SSDT_Table->setColumnCount(4);
    ui->SSDT_Table->setRowCount(0);
    ui->SSDT_Table->verticalHeader()->setDefaultSectionSize(15);
    ui->SSDT_Table->setColumnWidth(0, 40);
    ui->SSDT_Table->setColumnWidth(1, 320);
    ui->SSDT_Table->setColumnWidth(2, 120);
    ui->SSDT_Table->setHorizontalHeaderLabels(labels);
    ui->SSDT_Table->setSortingEnabled(true);
    ui->SSDT_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->SSDT_Table->setSelectionBehavior(QAbstractItemView::SelectRows);

    return true;
}

int ArkTools::m_GetSSDTCount()
{
    DWORD dwSSDTCount = 0;
    DWORD bytesOfRead = 0;
    DeviceIoControl(m_hFile, IOCTRL_GET_SSDT_COUNT, NULL, 0, &dwSSDTCount,sizeof(DWORD), &bytesOfRead, NULL);
    qDebug() << "[ArkTools_Ring3]m_GetSSDTCount:" << dwSSDTCount;

    if(dwSSDTCount > 0)
    {
        m_GetSSDTData(dwSSDTCount);

    }

    return 0;
}

int ArkTools::m_GetSSDTData(DWORD dwSSDTCount)
{
    DWORD bytesOfRead = 0;
    int nRowCont = 0;
    unsigned int* Buffer = new unsigned int[dwSSDTCount];
    DeviceIoControl(m_hFile, IOCTRL_GET_SSDT_INFO, NULL, 0, Buffer,dwSSDTCount * sizeof(DWORD), &bytesOfRead, NULL);

    if(bytesOfRead == dwSSDTCount * sizeof(DWORD))
    {
        ui->SSDT_Label->setText(QStringLiteral("SSDT函数:")+QString::number(dwSSDTCount));
        for(int i = 0;i < dwSSDTCount;i++)
        {
            nRowCont = ui->SSDT_Table->rowCount();
            ui->SSDT_Table->insertRow(nRowCont);//增加一行
            ui->SSDT_Table->setItem(nRowCont,0,new QTableWidgetItem(QString::number(i)));
            ui->SSDT_Table->setItem(nRowCont,1,new QTableWidgetItem(g_SSDT[i]));
            ui->SSDT_Table->setItem(nRowCont,2,new QTableWidgetItem("0x" + tr("%1").arg(*(Buffer + i),8,16,QLatin1Char('0')).toUpper()));
        }
    }

    if(Buffer != nullptr)
    {
        delete[] Buffer;
    }

    return 0;
}
