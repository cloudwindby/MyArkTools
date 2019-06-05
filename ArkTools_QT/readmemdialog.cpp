#include "readmemdialog.h"
#include "ui_readmemdialog.h"

ReadMemDialog::ReadMemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReadMemDialog)
{
    ui->setupUi(this);

}

ReadMemDialog::~ReadMemDialog()
{
    delete ui;
}

void ReadMemDialog::showEvent(QShowEvent *event)
{
    ui->PID_Label->setText(QString::number(PID()));
    qDebug() << "[ArkTools_Ring3] ReadMemDialog_PID:" << m_PID;
}

void ReadMemDialog::on_pushButton_clicked()
{
    //申请一个结构体.把整个结构体传进去.结构体最后一个成员定义成柔性数组.
        tagMemContent pMem = { 0 };//new tagMemContent();
        //char* pContent = new char[0x400];
        char buf[1024];
       // sscanf(strAddr, "%x", &(pMem.nAddress));
        //pMem.pContent = pContent;
        pMem.nSize = 0x400;
        //pMem.nLength = _ttoi(strLength);
        //pMem.nPID = _ttoi(strPID);

        // 读取内存
        DWORD dwBytes = 0;

        //传数据传一个结构体大小.获取数据,前面是结构体的内容,后面是柔性数组的内容.
//        NTSTATUS status = DeviceIoControl(m_hDriver, IOCTRL_MEMREAD, &pMem,
//                                          sizeof(tagMemContent), buf, sizeof(tagMemContent)+0x400, &dwBytes, NULL);

        //新建个buffer获取RING0的数据
        tagMemContent* pM = (tagMemContent*)buf;


        char buftemp[256];//格式化输出字符串用
}

ULONG ReadMemDialog::PID() const
{
    return m_PID;
}

void ReadMemDialog::setPID(const ULONG &PID)
{
    m_PID = PID;
}

HANDLE ReadMemDialog::hFile() const
{
    return m_hFile;
}

void ReadMemDialog::setHFile(const HANDLE &hFile)
{
    m_hFile = hFile;
}
