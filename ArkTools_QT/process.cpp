#include "arktools.h"
#include "ui_arktools.h"
#include <shlwapi.h>
#include <strsafe.h>
#include <QFileIconProvider>

bool ArkTools::m_InitProcessTable()
{
    QStringList labels;
    labels << QStringLiteral("映像名称")
           << QStringLiteral("进程ID")
           << QStringLiteral("父进程ID")
           << QStringLiteral("映像路径")
           << QStringLiteral("EPROCESS")
           << QStringLiteral("DirBase")
           << QStringLiteral("文件厂商");

    ui->EPROCESS_Table->setColumnCount(7);
    ui->EPROCESS_Table->setRowCount(0);
    ui->EPROCESS_Table->verticalHeader()->setDefaultSectionSize(20);
    ui->EPROCESS_Table->setColumnWidth(0, 120);
    ui->EPROCESS_Table->setColumnWidth(1, 60);
    ui->EPROCESS_Table->setColumnWidth(2, 70);
    ui->EPROCESS_Table->setColumnWidth(3, 300);
    ui->EPROCESS_Table->setColumnWidth(4, 80);
    ui->EPROCESS_Table->setColumnWidth(5, 80);
    ui->EPROCESS_Table->setHorizontalHeaderLabels(labels);
    ui->EPROCESS_Table->setSortingEnabled(true);
    ui->EPROCESS_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->EPROCESS_Table->setSelectionBehavior(QAbstractItemView::SelectRows);

    return true;
}

BOOLEAN ArkTools::m_NtPathToDosPathW(WCHAR *FullNtPath, WCHAR *FullDosPath)
{
    WCHAR DosDevice[4] = { 0 };       //dos设备名最大长度为4
    WCHAR NtPath[64] = { 0 };       //nt设备名最大长度为64
    WCHAR *RetStr = NULL;
    size_t NtPathLen = 0;
    if (!FullNtPath || !FullDosPath)
    {
        return FALSE;
    }
    for (short i = 65; i<26 + 65; i++)
    {
        DosDevice[0] = i;
        DosDevice[1] = L':';
        if (QueryDosDeviceW(DosDevice, NtPath, 64))
        {
            if (NtPath)
            {
                NtPathLen = wcslen(NtPath);
                if (!wcsnicmp(NtPath, FullNtPath, NtPathLen))
                {
                    wcscpy(FullDosPath, DosDevice);
                    wcscat(FullDosPath, FullNtPath + NtPathLen);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

DWORD ArkTools::GetPID()
{
    int nIndex = ui->EPROCESS_Table->currentRow();
    qDebug() << "[ArkTools_Ring3] CurrentRow:" << nIndex;
    QTableWidgetItem *pItem = ui->EPROCESS_Table->item(nIndex,1);
    QString qsPID = pItem->text();
    // m_ProcessName = ui->EPROCESS_Table->item(nIndex,0)->text();
    qDebug() << "[ArkTools_Ring3] PID:" << qsPID;
    return qsPID.toUInt();
}

QString ArkTools::GetProcessName()
{
    int nIndex = ui->EPROCESS_Table->currentRow();
    QTableWidgetItem *pItem = ui->EPROCESS_Table->item(nIndex,0);
    return pItem->text();
}

QString ArkTools::m_GetEPROCESS()
{
    bool b;
    int nIndex = ui->EPROCESS_Table->currentRow();
    QTableWidgetItem *pItem = ui->EPROCESS_Table->item(nIndex,4);
    return pItem->text();
}

int ArkTools::m_GetProcessInfo()
{
    DWORD bytesOfRead = 0;
    int nRowCont;
    int i = 0;
    ALLPROCESSINFO tagPsInfo;

    memset(&tagPsInfo,0,sizeof(ALLPROCESSINFO));

    WCHAR wchNTFileFullPath[256];
    WCHAR wchDOSFileFullPath[256];
    DeviceIoControl(m_hFile, IOCTRL_GET_EPROCESS, NULL, 0, &tagPsInfo,sizeof(tagPsInfo), &bytesOfRead, NULL);
    qDebug() << "[ArkTools_Ring3]bytesOfRead:" << bytesOfRead;
    if(bytesOfRead == 0)
    {
        ui->STATUS_Label->setText(QStringLiteral("错误:未读取到数据."));
        return 1;
    }

    PALLPROCESSINFO pAllPsInfo = &tagPsInfo;
    //qDebug() << "[ArkTools_Ring3]ProcessCount:" << pAllPsInfo->ProcessCount;

    for(i = 0;i < (bytesOfRead - 4)/sizeof(PROCESSINFO);i++)
    {
        //clean string buffer
        memset(wchNTFileFullPath,0,sizeof(wchNTFileFullPath));
        memset(wchDOSFileFullPath,0,sizeof(wchDOSFileFullPath));

        nRowCont = ui->EPROCESS_Table->rowCount();
        ui->EPROCESS_Table->insertRow(nRowCont);//增加一行

        ui->EPROCESS_Table->setItem(nRowCont,1,new QTableWidgetItem(QString::number(pAllPsInfo->PsInfo[i].CurrentPid)));

        ui->EPROCESS_Table->setItem(nRowCont,5,new QTableWidgetItem("0x" + tr("%1").arg(pAllPsInfo->PsInfo[i].DirBase,8,16,QLatin1Char('0')).toUpper()));
        ui->EPROCESS_Table->setItem(nRowCont,4,new QTableWidgetItem("0x" + tr("%1").arg(pAllPsInfo->PsInfo[i].EprocessAddr,8,16,QLatin1Char('0')).toUpper()));

        if(pAllPsInfo->PsInfo[i].CurrentPid == 0)
        {
            ui->EPROCESS_Table->setItem(nRowCont,2,new QTableWidgetItem("-"));
            ui->EPROCESS_Table->setItem(nRowCont,0,new QTableWidgetItem("Idle"));
            ui->EPROCESS_Table->setItem(nRowCont,3,new QTableWidgetItem("Idle"));
        }
        else if(pAllPsInfo->PsInfo[i].CurrentPid == 4)
        {
            ui->EPROCESS_Table->setItem(nRowCont,2,new QTableWidgetItem("-"));
            ui->EPROCESS_Table->setItem(nRowCont,0,new QTableWidgetItem("System"));
            ui->EPROCESS_Table->setItem(nRowCont,3,new QTableWidgetItem("System"));
        }
        else
        {
            ui->EPROCESS_Table->setItem(nRowCont,2,new QTableWidgetItem(QString::number(pAllPsInfo->PsInfo[i].ParentPid)));
            //qDebug("[ArkTools_Ring3]ProcessFilePath:%ls",pAllPsInfo->PsInfo[i].FilePath);
            QString FileFullPath(QString::fromStdWString(pAllPsInfo->PsInfo[i].FilePath));
            //qDebug() << "[ArkTools_Ring3]QString:FilePath:%ls" << FileFullPath;
            int  i = FileFullPath.lastIndexOf("\\");
            QString ImagePath = FileFullPath.right(FileFullPath.length() - i - 1);
            //QString k = FileFullPath.left(i + 1);
            //qDebug() << "[ArkTools_Ring3]QString:ImagePath:%ls" << ImagePath;


            FileFullPath.toWCharArray(wchNTFileFullPath);
            //qDebug("[ArkTools_Ring3]wchFileFullPath:%ls",wchNTFileFullPath);
            m_NtPathToDosPathW(wchNTFileFullPath,wchDOSFileFullPath);
            //qDebug("[ArkTools_Ring3]wchFileFullPath:%ls",wchDOSFileFullPath);
            QString qstrFilePath(QString::fromWCharArray(wchDOSFileFullPath));


            QTableWidgetItem *ImageName = new QTableWidgetItem(ImagePath);
            QFileInfo file_info(qstrFilePath);
            QFileIconProvider icon_provider;
            ImageName->setIcon(icon_provider.icon(file_info));

            ui->EPROCESS_Table->setItem(nRowCont,0,ImageName);

            QTableWidgetItem *cubesHeaderItem = new QTableWidgetItem(qstrFilePath);
            ui->EPROCESS_Table->setItem(nRowCont,3,cubesHeaderItem);
            ui->EPROCESS_Table->setItem(nRowCont,6,new QTableWidgetItem(m_GetCompanyName(qstrFilePath)));
        }
    }
    ui->STATUS_Label->setText(QStringLiteral("进程:")+ QString::number(i));

    return 0;
}

QString ArkTools::m_GetCompanyName(QString qstrFilePath)
{
    QString qstrCompanyName = 0;
    //qDebug() << "[ArtTools_Ring3]Process:CompanyName:" << qstrFilePath;
    //qDebug() << "[ArtTools_Ring3]qstrFilePath:" << qstrFilePath;

    if (qstrFilePath.isEmpty())
    {
        return NULL;
    }
    if (!PathFileExists(qstrFilePath.toStdWString().c_str()))
    {
        return qstrCompanyName;
    }

    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate;

    WCHAR lpstrFilename[256] = {0};
    qstrFilePath.toWCharArray(lpstrFilename);

    //qDebug("[ArtTools_Ring3]lpstrFilename:%ls",lpstrFilename);
    DWORD dwHandle = 0;
    DWORD dwVerInfoSize = GetFileVersionInfoSize(lpstrFilename, &dwHandle);

    //qDebug() << "[ArtTools_Ring3]GetFileVersionInfoSize_GetLastError:" << GetLastError();

    //qDebug() << "[ArtTools_Ring3]GetFileVersionInfoSize_dwVerInfoSize:" << dwVerInfoSize;
    // Dos FileHead
    //[][][][][][][][]   [][][][]

    if (dwVerInfoSize != NULL)
    {
        LPVOID Buffer = LocalAlloc(LMEM_ZEROINIT, dwVerInfoSize);  //1852

        if (Buffer)
        {
            if (GetFileVersionInfo(lpstrFilename, dwHandle, dwVerInfoSize, Buffer))
            {
                //qDebug() << "[ArtTools_Ring3]GetFileVersionInfo_GetLastError:" << GetLastError();
                UINT cbTranslate = 0;

                if ( VerQueryValue(Buffer, L"\\VarFileInfo\\Translation", (LPVOID*) &lpTranslate, &cbTranslate))
                {
                    LPCWSTR lpwszBlock = 0;
                    UINT    cbSizeBuf  = 0;
                    WCHAR   wzSubBlock[MAX_PATH] = {0};  //260

                    if ((cbTranslate/sizeof(struct LANGANDCODEPAGE)) > 0)
                    {
                        StringCchPrintf(wzSubBlock, sizeof(wzSubBlock)/sizeof(WCHAR),
                                        L"\\StringFileInfo\\%04x%04x\\CompanyName", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
                    }

                    //"\\VarFileInfo\\Translation

                    if ( VerQueryValue(Buffer, wzSubBlock, (LPVOID*)&lpwszBlock, &cbSizeBuf))
                    {
                        WCHAR wzCompanyName[MAX_PATH] = {0};

                        StringCchCopy(wzCompanyName, MAX_PATH/sizeof(WCHAR), (LPCWSTR)lpwszBlock);
                        qstrCompanyName = QString::fromWCharArray(wzCompanyName);
                        //qDebug() << "[ArtTools_Ring3]Process:CompanyName:" << qstrCompanyName;
                    }
                }
            }

            LocalFree(Buffer);
        }
    }

    return qstrCompanyName;
}

void ArkTools::slot_readmem()
{
    m_ReadmemDialog = QSharedPointer<ReadMemDialog>(new ReadMemDialog());
    m_ReadmemDialog->setHFile(m_hFile);
    m_ReadmemDialog->setPID(GetPID());
    m_ReadmemDialog->show();
    qDebug("[ArkTools_Ring3]slot_readmem");
}

void ArkTools::slot_terminate()
{
    //    HANDLE hProcess =  OpenProcess(PROCESS_TERMINATE,FALSE,GetPID());
    //        if(hProcess == 0)
    //        {
    //            QMessageBox::warning(NULL, "warning", "OpenProcess Error", QMessageBox::Ok);
    //            return;
    //        }

    //        if(TerminateProcess(hProcess,0))
    //        {
    //            Sleep(1000);
    //            slot_refresh();
    //            return;
    //        }
    //        else
    //        {
    //            QMessageBox::warning(NULL, "warning", "TerminateProcess Error", QMessageBox::Ok);
    //            return;
    //        }
}

void ArkTools::slot_terminatesystem()
{

}

void ArkTools::slot_refresh()
{
    ui->EPROCESS_Table->clear();
    m_InitProcessTable();
    m_GetProcessInfo();
}

void ArkTools::slot_thread()
{
    m_ThreadDialog = QSharedPointer<ThreadDialog>(new ThreadDialog());
    bool b;
    qDebug() << "QString:ERPOCESS:" << m_GetEPROCESS();
    m_ThreadDialog->setTitle(GetProcessName());
    m_ThreadDialog->setEPROCESS(m_GetEPROCESS().toLongLong(&b,16));
    m_ThreadDialog->setHFile(m_hFile);
    m_ThreadDialog->exec();
    //m_ThreadDialog->show();
}
