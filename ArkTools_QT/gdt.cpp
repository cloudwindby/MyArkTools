#include "arktools.h"
#include "ui_arktools.h"
#include "gdt.h"

bool ArkTools::m_InitGdtTable()
{
    QStringList labels;
    labels << QStringLiteral("CPU序号")
           << QStringLiteral("段选择子(Index)")
           << QStringLiteral("基址")
           << QStringLiteral("界限")
           << QStringLiteral("段粒度")
           << QStringLiteral("段特权级")
           << QStringLiteral("类型");

    ui->GDT_Table->setColumnCount(7);
    ui->GDT_Table->setRowCount(0);
    ui->GDT_Table->verticalHeader()->setDefaultSectionSize(15);
    ui->GDT_Table->setColumnWidth(1, 120);
    ui->GDT_Table->setColumnWidth(4, 50);
    ui->GDT_Table->setColumnWidth(5, 60);
    ui->GDT_Table->setHorizontalHeaderLabels(labels);
    ui->GDT_Table->setSortingEnabled(true);
    ui->GDT_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->GDT_Table->setSelectionBehavior(QAbstractItemView::SelectRows);

    return true;
}

int ArkTools::m_ShowGDT()
{
    int nCurrentProcessor;

    for(int j = 0;j < m_si.dwNumberOfProcessors;j++)
    {
        nCurrentProcessor = j;
        DWORD bytesOfRead = 0;
        DeviceIoControl(m_hFile, IOCTL_HELLO_WORLD, &nCurrentProcessor, sizeof(nCurrentProcessor), m_szGDT,1024, &bytesOfRead, NULL);

        if(bytesOfRead == 0)
        {
            continue;
        }
        PGDTENTRY p = (PGDTENTRY)m_szGDT;
        for(int i = 0;i < 128;i++)
        {

            if((p+i)->P != 0)
            {
                int nRowCont;
                nRowCont=ui->GDT_Table->rowCount();
                ui->GDT_Table->insertRow(nRowCont);//增加一行

                ui->GDT_Table->setItem(nRowCont,0,new QTableWidgetItem(QString::number(j)));

                if((p+i)->S == 1)
                {
                    if((p+i)->Type >= 8)
                    {
                        ui->GDT_Table->setItem(nRowCont,6,new QTableWidgetItem(QStringLiteral("代码段:")+QString(QString::fromLocal8Bit(g_Atrr[(p+i)->Type]))));
                    }
                    else
                    {
                        ui->GDT_Table->setItem(nRowCont,6,new QTableWidgetItem(QStringLiteral("数据段:")+QString(QString::fromLocal8Bit(g_Atrr[(p+i)->Type]))));
                    }
                }
                else
                {
                    ui->GDT_Table->setItem(nRowCont,6,new QTableWidgetItem(QStringLiteral("系统段:")+QString(QString::fromLocal8Bit(g_SystemAtrr[(p+i)->Type]))));
                }

                ui->GDT_Table->setItem(nRowCont,1,new QTableWidgetItem("0x" + tr("%1").arg(i,4,16,QLatin1Char('0')).toUpper()));

                unsigned int seg_base =
                        ((p+i)->BaseHi << 24)|((p+i)->BaseMid <<16 )|(p+i)->BaseLow;//拼接base

                unsigned int seg_limit =
                        ((p+i)->LimitHi << 16)|(p+i)->LimitLow;//拼接limit

                QString dpl = QString::number((p+i)->Dpl,10);
                ui->GDT_Table->setItem(nRowCont,2,new QTableWidgetItem("0x" + tr("%1").arg(seg_base,8,16,QLatin1Char('0')).toUpper()));

                if((p+i)->G == 1)
                {
                    ui->GDT_Table->setItem(nRowCont,3,new QTableWidgetItem("0x" + tr("%1").arg(seg_limit,5,16,QLatin1Char('0')).toUpper() + "FFF"));
                    ui->GDT_Table->setItem(nRowCont,4,new QTableWidgetItem("Page"));
                }
                else
                {
                    ui->GDT_Table->setItem(nRowCont,3,new QTableWidgetItem("0x" + tr("%1").arg(seg_limit,8,16,QLatin1Char('0')).toUpper()));
                    ui->GDT_Table->setItem(nRowCont,4,new QTableWidgetItem("Byte"));
                }

                ui->GDT_Table->setItem(nRowCont,5,new QTableWidgetItem(dpl));
            }
        }
    }
    return 0;
}
