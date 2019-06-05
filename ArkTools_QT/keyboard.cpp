#include "dispathmajorfunc.h"
#include "arktools.h"
#include "ui_arktools.h"

bool ArkTools::m_InitKeyBoardMajorFuncTable(PVOID pTWidGet)
{
    QTableWidget* pTableWidGet = (QTableWidget*)pTWidGet;
    QStringList labels;
    labels << QStringLiteral("序号")
           << QStringLiteral("函数名称")
           << QStringLiteral("当前函数地址")
           << QStringLiteral("HOOK")
           << QStringLiteral("原始函数地址")
           << QStringLiteral("当前函数地址所在模块");

//    ui->KEYBOARD_Table->setColumnCount(6);
//    ui->KEYBOARD_Table->setRowCount(0);
//    ui->KEYBOARD_Table->verticalHeader()->setDefaultSectionSize(15);
//    ui->KEYBOARD_Table->setColumnWidth(0, 40);
//    ui->KEYBOARD_Table->setColumnWidth(1, 320);
//   ui->KEYBOARD_Table->setColumnWidth(2, 120);
//    ui->KEYBOARD_Table->setHorizontalHeaderLabels(labels);
//    ui->KEYBOARD_Table->setSortingEnabled(true);
//    ui->KEYBOARD_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    ui->KEYBOARD_Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTableWidGet->setColumnCount(6);
    pTableWidGet->setRowCount(0);
    pTableWidGet->verticalHeader()->setDefaultSectionSize(15);
    pTableWidGet->setColumnWidth(0, 40);
    pTableWidGet->setColumnWidth(1, 320);
    pTableWidGet->setColumnWidth(2, 120);
    pTableWidGet->setHorizontalHeaderLabels(labels);
    pTableWidGet->setSortingEnabled(true);
    pTableWidGet->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pTableWidGet->setSelectionBehavior(QAbstractItemView::SelectRows);

    return true;
}

int ArkTools::m_GetKeyBoardMajorFuncData(PVOID pTWidGet,DWORD IOCTRL_CODE)
{
    QTableWidget* pTableWidGet = (QTableWidget*)pTWidGet;
    DWORD bytesOfRead;
    int nRowCont = 0;
    ULONG DispathFuncAddr[28] = {0};
    DeviceIoControl(m_hFile,IOCTRL_CODE,NULL,0,DispathFuncAddr,28 * sizeof(ULONG),&bytesOfRead,NULL);

    if(bytesOfRead == 28 * sizeof(ULONG))
    {
        for(int i = 0;i < 28;i++)
        {
            //qDebug("[ArkTools_Ring3]DispatchOrdinal:%d DispatchFuncAddr:%p\n",i,DispathFuncAddr[i]);
            nRowCont = pTableWidGet->rowCount();
            pTableWidGet->insertRow(nRowCont);//增加一行
            pTableWidGet->setItem(nRowCont,0,new QTableWidgetItem(QString::number(i)));
            pTableWidGet->setItem(nRowCont,1,new QTableWidgetItem(g_MajorFunc[i]));
            pTableWidGet->setItem(nRowCont,4,new QTableWidgetItem("0x" + tr("%1").arg(DispathFuncAddr[i],8,16,QLatin1Char('0')).toUpper()));
        }
    }

    return 0;
}
