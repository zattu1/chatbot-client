#include <QCoreApplication>
#include "clogthread.h"
#include "mainwindow.h"

CLogThread::CLogThread()
{
    m_strFilePath = QString("%1%2Log").arg(QCoreApplication::applicationDirPath()).arg(QDir::separator());
    m_dir = QDir(m_strFilePath);
    MakeDir(m_strFilePath);
}

void CLogThread::writeLog(QString strMessage)
{
    QFile file(QString("%1%2TraceLog1.txt").arg(m_strFilePath).arg(QDir::separator()));
    bool bRet = true;
    if(file.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Append)) {
        if(ChkFileSize(file))
            bRet = file.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Append);
        if(bRet) {
            QTextStream out(&file);
            out << strMessage << Qt::endl;
        }
    }
}

void CLogThread::MakeDir(QString sPath)
{
    for(int ii=0; ii<sPath.length(); ii++) {
        if(sPath[ii] == '\\' || sPath[ii] == '/') {
            if(ii > 1 && sPath[ii-1] != ':') {
                if(!m_dir.exists(sPath.left(ii)))
                    m_dir.mkdir(sPath.left(ii));
            }
        }
    }
    if(!m_dir.exists(sPath))
        m_dir.mkdir(sPath);
}

bool CLogThread::ChkFileSize(QFile &file)
{
    if(file.size() > (1024*1024*5)) {   // 5Mbyte
        file.close();
        FileRename();
        return true;
    }
    return false;
}

void CLogThread::FileRename()
{
    // 最大ファイルを削除
    QStringList fileList = m_dir.entryList(QStringList("TraceLog10.txt"), QDir::Files);
    if(!fileList.isEmpty())
        m_dir.remove(fileList[0]);
    for(int ii=9; ii>0; ii--) {
        fileList = m_dir.entryList(QStringList(QString("TraceLog%1.txt").arg(ii)), QDir::Files);
        if(!fileList.isEmpty())
            m_dir.rename(fileList[0], QString("TraceLog%1.txt").arg(ii+1));
    }
}

CTraceLog::CTraceLog(QObject *parent) :
    QObject(parent),
    m_pMainWindow(static_cast<MainWindow *>(parent))
{
    m_pLogThread = new CLogThread;
    m_pRealThread = new QThread;
    m_pLogThread->moveToThread(m_pRealThread);
    QObject::connect(m_pRealThread, SIGNAL(finished()), m_pRealThread, SLOT(deleteLater()));
    QObject::connect(this, SIGNAL(logOut(QString)), m_pLogThread, SLOT(writeLog(QString)), Qt::QueuedConnection);
    m_pRealThread->start();
}

void CTraceLog::DbgPrint(QString str, bool bStatusBar)
{
    QDateTime dt = QDateTime::currentDateTime();
    QString strTime = QString("%1(%2) %3").arg(dt.toString("yyyy-MM-dd hh:mm:ss.zzz"))
#if defined(Q_OS_WIN)
                          .arg((qint64)QThread::currentThreadId())
#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
                          .arg((qint64)QThread::currentThread())
#endif
                          .arg(str);
#ifndef _RELEASE
    qDebug() << strTime;
#endif
    emit logOut(strTime);
    if(bStatusBar && m_pMainWindow->isWindow()) {
        emit showStatusMessage(strTime, 60000);
    }
}
