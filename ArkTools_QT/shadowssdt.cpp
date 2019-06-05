#include "arktools.h"
#include "ui_arktools.h"
#include "ShadowSSDT.h"

bool ArkTools::m_InitSSSDTTable()
{
    QStringList labels;
    labels << QStringLiteral("序号")
           << QStringLiteral("函数名")
           << QStringLiteral("地址")
           << QStringLiteral("模块");

    ui->SSSDT_Table->setColumnCount(4);
    ui->SSSDT_Table->setRowCount(0);
    ui->SSSDT_Table->verticalHeader()->setDefaultSectionSize(15);
    ui->SSSDT_Table->setColumnWidth(0, 40);
    ui->SSSDT_Table->setColumnWidth(1, 280);
    ui->SSSDT_Table->setColumnWidth(2, 120);
    ui->SSSDT_Table->setHorizontalHeaderLabels(labels);
    ui->SSSDT_Table->setSortingEnabled(true);
    ui->SSSDT_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->SSSDT_Table->setSelectionBehavior(QAbstractItemView::SelectRows);

    return true;
}

int ArkTools::m_GetShadowSSDTCount()
{
    DWORD dwShadowSDTCount = 0;
    DWORD bytesOfRead = 0;
    DeviceIoControl(m_hFile, IOCTRL_GET_ShadowSDT_COUNT, NULL, 0, &dwShadowSDTCount,sizeof(DWORD), &bytesOfRead, NULL);
    qDebug() << "[ArkTools_Ring3]m_GetShadowSSDTCount:" << dwShadowSDTCount;

    if(dwShadowSDTCount > 0)
    {
        m_GetShadowSSDTData(dwShadowSDTCount);
    }

    return 0;
}

int ArkTools::m_GetShadowSSDTData(DWORD dwShadowSDTCount)
{
    DWORD bytesOfRead = 0;
    int nRowCont = 0;
    unsigned int* Buffer = new unsigned int[dwShadowSDTCount];
    DeviceIoControl(m_hFile, IOCTRL_GET_ShadowSSDT_INFO, NULL, 0, Buffer,dwShadowSDTCount * sizeof(DWORD), &bytesOfRead, NULL);


    if(bytesOfRead == dwShadowSDTCount * sizeof(DWORD))
    {
        ui->SSSDT_Lable->setText(QStringLiteral("ShadowSSDT函数:")+QString::number(dwShadowSDTCount));
        for(int i = 0;i < dwShadowSDTCount;i++)
        {
            nRowCont = ui->SSSDT_Table->rowCount();
            ui->SSSDT_Table->insertRow(nRowCont);//增加一行
            //qDebug("[ArkTools_Ring3]SSSDTData:%p",*(Buffer + i));

            ui->SSSDT_Table->setItem(nRowCont,0,new QTableWidgetItem(QString::number(i)));
            ui->SSSDT_Table->setItem(nRowCont,1,new QTableWidgetItem(g_SSSDT[i]));
            ui->SSSDT_Table->setItem(nRowCont,2,new QTableWidgetItem("0x" + tr("%1").arg(*(Buffer + i),8,16,QLatin1Char('0')).toUpper()));
        }
    }

    if(Buffer != nullptr)
    {
        delete[] Buffer;
    }

    return 0;
}


