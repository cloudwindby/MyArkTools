#ifndef READMEMDIALOG_H
#define READMEMDIALOG_H

#include <QDialog>
#include <windows.h>
#include <qDebug>
#include "../common/common.h"

namespace Ui {
class ReadMemDialog;
}

class ReadMemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReadMemDialog(QWidget *parent = 0);
    ~ReadMemDialog();
    virtual void showEvent(QShowEvent *event);

    HANDLE hFile() const;
    void setHFile(const HANDLE &hFile);

    ULONG PID() const;
    void setPID(const ULONG &PID);

private slots:
    void on_pushButton_clicked();

private:
    Ui::ReadMemDialog *ui;
    HANDLE m_hFile;
    ULONG m_PID;
};

#endif // READMEMDIALOG_H
