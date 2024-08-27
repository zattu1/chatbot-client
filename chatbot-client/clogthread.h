#ifndef CLOGTHREAD_H
#define CLOGTHREAD_H

#include <QMainWindow>
#include <QThread>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>

class CLogThread : public QObject
{
    Q_OBJECT
public:
    CLogThread();

public slots:
    void writeLog(QString strMessage);

private:
    void MakeDir(QString sPath);
    bool ChkFileSize(QFile &file);
    void FileRename();

    QString m_strFilePath;
    QDir m_dir;
};

class MainWindow;
class CTraceLog : public QObject
{
    Q_OBJECT
public:
    CTraceLog(QObject *parent);
    void DbgPrint(QString str, bool bStatusBar = true);

signals:
    void logOut(QString log);
    void showStatusMessage(QString, int);

private:
    CLogThread *m_pLogThread;
    QThread *m_pRealThread;
    MainWindow *m_pMainWindow;
};

#endif // CLOGTHREAD_H
