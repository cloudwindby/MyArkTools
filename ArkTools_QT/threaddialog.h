#ifndef THREADDIALOG_H
#define THREADDIALOG_H

#include <QDialog>
#include <windows.h>



namespace Ui {
class ThreadDialog;
}

class ThreadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ThreadDialog(QWidget *parent = 0);
    ~ThreadDialog();
    virtual void showEvent(QShowEvent *event);
    bool m_InitThreadTable();
    void setTitle(const QString &ProcessName);
    int GetThreadData();

    long long EPROCESS() const;
    void setEPROCESS(const long long &EPROCESS);

    HANDLE hFile() const;
    void setHFile(const HANDLE &hFile);

private:
    Ui::ThreadDialog *ui;
    long long m_EPROCESS;
    HANDLE m_hFile;
    QString m_ProcessName;
};

#endif // THREADDIALOG_H
