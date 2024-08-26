#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setEnableWindow(false);
    getRegistration();
    connect(ui->emailLineEdit, SIGNAL(editingFinished()), this, SLOT(onLogin()));
    connect(ui->passLineEdit, SIGNAL(editingFinished()), this, SLOT(onPassword()));
    on_comboBox_currentIndexChanged(0);

    m_pWebSocket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    connect(m_pWebSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_pWebSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_pWebSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(m_pWebSocket, SIGNAL(handshakeInterruptedOnError(QSslError)), this, SLOT(onHandshakeError(QSslError)));
    connect(m_pWebSocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onReceiveData(QByteArray)));
    connect(m_pWebSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onReceiveMsg(QString)));
}

MainWindow::~MainWindow()
{
    m_pWebSocket->deleteLater();
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    setRegistration();
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    ui->emailLineEdit->setText(m_personalInformation[index].m_login);
    ui->passLineEdit->setText(m_personalInformation[index].m_pass);
}

void MainWindow::setRegistration()
{
    QSettings settings("zattuRight", EXENAME);

    for(int ii=0; ii<MAX_RESERVE; ii++) {
        settings.beginGroup(QString("%1").arg(ii+1));
        settings.setValue("login", m_personalInformation[ii].m_login);
        settings.setValue("Pass", m_personalInformation[ii].m_pass);
        settings.endGroup();
    }

    QString strRemote = settings.value("remote").toString();
    if(!strRemote.isEmpty()) {
        ui->remoteLineEdit->setText(strRemote);
    }
}

void MainWindow::getRegistration()
{
    QSettings settings("zattuRight", EXENAME);

    QStringList list = settings.childGroups();
    for(int ii=0; ii<list.count(); ii++) {
        int nIndex = list[ii].toInt();
        if(nIndex > 0 && nIndex <= MAX_RESERVE) {
            settings.beginGroup(list[ii]);
            m_personalInformation[nIndex-1].m_login = settings.value("login").toString();
            m_personalInformation[nIndex-1].m_pass = settings.value("Pass").toString();
            settings.endGroup();
        }
    }

    settings.setValue("remote", ui->remoteLineEdit->text());
}

void MainWindow::onEmail()
{
    m_personalInformation[ui->comboBox->currentIndex()].m_login = ui->emailLineEdit->text();
}

void MainWindow::onPassword()
{
    m_personalInformation[ui->comboBox->currentIndex()].m_pass = ui->passLineEdit->text();
}

void MainWindow::on_connPushButton_clicked()
{
    if(ui->connPushButton->text() == "接続") {
        m_pWebSocket->open(ui->remoteLineEdit->text());
        ui->remoteLineEdit->setEnabled(false);
        ui->connPushButton->setEnabled(false);
    }else {
        m_pWebSocket->close();
        ui->remoteLineEdit->setEnabled(true);
        ui->connPushButton->setEnabled(true);
    }
}

void MainWindow::on_loginPushButton_clicked()
{
    if(ui->loginPushButton->text() == "ログイン") {
        ui->comboBox->setEnabled(false);
        ui->emailLineEdit->setEnabled(false);
        ui->passLineEdit->setEnabled(false);

        QJsonObject root;
        root["id"] = 1; // ログイン
        root["email"] = ui->emailLineEdit->text();
        root["password"] = ui->passLineEdit->text();
        QJsonDocument d(root);
        m_pWebSocket->sendBinaryMessage(d.toJson(QJsonDocument::Compact));
    }else {
        QJsonObject root;
        root["id"] = 4; // ログオフ
        QJsonDocument d(root);
        m_pWebSocket->sendBinaryMessage(d.toJson(QJsonDocument::Compact));
    }
}


void MainWindow::on_historyPushButton_clicked()
{

}


void MainWindow::on_sendPushButton_clicked()
{

}

void MainWindow::onConnected()
{
    setEnableWindow(true);
    ui->connPushButton->setText("切断");
}

void MainWindow::onDisconnected()
{
    setEnableWindow(false);
}

void MainWindow::onError(QAbstractSocket::SocketError error)
{

}

void MainWindow::onHandshakeError(const QSslError &error)
{
    setEnableWindow(false);
}

void MainWindow::onReceiveMsg(QString message)
{

}

void MainWindow::onReceiveData(QByteArray message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message);
    if(!doc.isObject()) {
        return;
    }

    QJsonObject o = doc.object();
    int method = o["id"].toInt();
    switch(method) {
    case 1: {   // Login
        break; }
    case 2: {   // History
        break; }
    case 3: {   // Message
        break; }
    case 4: {   // Logout
        break; }
    case 5: {   // History set

    }
    default:
        break;
    }
}

void MainWindow::setEnableWindow(bool bEnable)
{
    QList<QWidget *> list = findChildren<QWidget *>();
    for(int ii=0; ii<list.count(); ii++) {
        if(list[ii]->objectName() != "label_4" &&
           list[ii]->objectName() != "remoteLineEdit" &&
           list[ii]->objectName() != "connPushButton") {
           list[ii]->setEnabled(bEnable);
        }
    }
}
