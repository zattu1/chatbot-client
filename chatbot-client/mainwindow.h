#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QListWidgetItem>
#include <QKeyEvent>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

#define MAX_RESERVE 10
#define EXENAME "chatbot-client"

class CPersonalInformation {
public:
    CPersonalInformation() = default;
    QString m_login;
    QString m_pass;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent * event) override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setRegistration();
    void getRegistration();
    void setEnableWindow(bool bEnable);
    void msleep(int msec) {
        QEventLoop loop;
        QTimer::singleShot(msec, &loop, SLOT(quit()));
        loop.exec();
    }

private slots:
    void on_requestTextEdit_textChanged();
    void on_comboBox_currentIndexChanged(int index);
    void on_historyListWidget_itemDoubleClicked(QListWidgetItem *item);
    void onEmail();
    void onPassword();
    void on_connPushButton_clicked();
    void on_loginPushButton_clicked();
    void on_historyPushButton_clicked();
    void on_sendPushButton_clicked();
    void onStatusChanged(QAbstractSocket::SocketState state);
    void onConnected();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void onHandshakeError(const QSslError &error);
    void onReceiveMsg(QString message);
    void onReceiveData(QByteArray message);

private:
    Ui::MainWindow *ui;
    CPersonalInformation m_personalInformation[MAX_RESERVE];
    QWebSocket *m_pWebSocket;
};
#endif // MAINWINDOW_H
