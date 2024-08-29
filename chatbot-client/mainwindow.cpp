#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include "clogthread.h"

CTraceLog *t;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    t = new CTraceLog(this);
    connect(t, SIGNAL(showStatusMessage(QString,int)), ui->statusbar, SLOT(showMessage(QString,int)));
    setEnableWindow(false);
    getRegistration();
    connect(ui->emailLineEdit, SIGNAL(editingFinished()), this, SLOT(onEmail()));
    connect(ui->passLineEdit, SIGNAL(editingFinished()), this, SLOT(onPassword()));
    on_comboBox_currentIndexChanged(0);

    m_pWebSocket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    QSslConfiguration sslConfiguration = QSslConfiguration::defaultConfiguration();
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);  // 自己署名証明書を許可
    m_pWebSocket->setSslConfiguration(sslConfiguration);
    //connect(m_pWebSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    //connect(m_pWebSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_pWebSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStatusChanged(QAbstractSocket::SocketState)));
    connect(m_pWebSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    connect(m_pWebSocket, SIGNAL(handshakeInterruptedOnError(QSslError)), this, SLOT(onHandshakeError(QSslError)));
    connect(m_pWebSocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onReceiveData(QByteArray)));
    connect(m_pWebSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onReceiveMsg(QString)));
}

MainWindow::~MainWindow()
{
    m_pWebSocket->deleteLater();
    delete ui;
    t->deleteLater();
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    Q_UNUSED(event)
    setRegistration();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return && (event->modifiers() & Qt::ControlModifier)) {
        qDebug() << "Ctrl+Enter was pressed!";
        // ここにCtrl+Enterが押されたときの処理を追加
        if(m_pWebSocket && !ui->requestTextEdit->toPlainText().isEmpty()) {
            msleep(100);
            on_sendPushButton_clicked();
        }
    } else {
        // 他のキーイベントは通常通り処理
        QWidget::keyPressEvent(event);
    }
}

void MainWindow::on_requestTextEdit_textChanged()
{
    ui->sendPushButton->setEnabled(!ui->requestTextEdit->toPlainText().isEmpty());
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    ui->emailLineEdit->setText(m_personalInformation[index].m_login);
    ui->passLineEdit->setText(m_personalInformation[index].m_pass);
}

void MainWindow::on_historyListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(item) {
        QJsonObject root;
        root["id"] = 5;     // 履歴表示
        root["url"] = item->data(Qt::UserRole).toString();
        QJsonDocument d(root);
        m_pWebSocket->sendBinaryMessage(d.toJson(QJsonDocument::Compact));
        t->DbgPrint(d.toJson(QJsonDocument::Compact), false);
    }
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

    QString strRemote = ui->remoteLineEdit->text();
    if(!strRemote.isEmpty()) {
        settings.setValue("remote", strRemote);
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

    ui->remoteLineEdit->setText(settings.value("remote").toString());
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
    t->DbgPrint("接続中...");
    ui->remoteLineEdit->setEnabled(false);
    ui->connPushButton->setEnabled(false);
    if(ui->connPushButton->text() == "接続") {
        m_pWebSocket->open(ui->remoteLineEdit->text());
    }else {
        m_pWebSocket->close();
    }
}

void MainWindow::on_loginPushButton_clicked()
{
    ui->loginPushButton->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->emailLineEdit->setEnabled(false);
    ui->passLineEdit->setEnabled(false);

    if(ui->loginPushButton->text() == "ログイン") {
        t->DbgPrint("ログイン施行中...");
        QJsonObject root;
        root["id"] = 1; // ログイン
        root["email"] = ui->emailLineEdit->text();
        root["password"] = ui->passLineEdit->text();
        QJsonDocument d(root);
        m_pWebSocket->sendBinaryMessage(d.toJson(QJsonDocument::Compact));
        t->DbgPrint(d.toJson(QJsonDocument::Compact), false);
    }else {
        t->DbgPrint("ログアウト施行中...");
        QJsonObject root;
        root["id"] = 4; // ログアウト
        QJsonDocument d(root);
        m_pWebSocket->sendBinaryMessage(d.toJson(QJsonDocument::Compact));
        t->DbgPrint(d.toJson(QJsonDocument::Compact), false);
    }
}

void MainWindow::on_historyPushButton_clicked()
{
    t->DbgPrint("履歴取得中...");
    ui->historyPushButton->setEnabled(false);
    ui->historyListWidget->setEnabled(false);
    QJsonObject root;
    root["id"] = 2; // 履歴取得
    QJsonDocument d(root);
    m_pWebSocket->sendBinaryMessage(d.toJson(QJsonDocument::Compact));
    t->DbgPrint(d.toJson(QJsonDocument::Compact), false);
}

void MainWindow::on_sendPushButton_clicked()
{
    t->DbgPrint("応答待ち...");
    ui->requestTextEdit->setEnabled(false);
    ui->sendPushButton->setEnabled(false);
    QJsonObject root;
    root["id"] = 3; // メッセージ送信
    root["message"] = ui->requestTextEdit->toPlainText();
    qDebug() << root["message"].toString();
    QJsonDocument d(root);
    m_pWebSocket->sendBinaryMessage(d.toJson(QJsonDocument::Compact));
    ui->requestTextEdit->setFocus();
    t->DbgPrint(d.toJson(QJsonDocument::Compact), false);
}

void MainWindow::onStatusChanged(QAbstractSocket::SocketState state)
{
    switch(state) {
    case QAbstractSocket::UnconnectedState:
        t->DbgPrint("接続状態：Unconnected");
        onDisconnected();
        break;
    case QAbstractSocket::ConnectingState:
        t->DbgPrint("接続状態：Connecting");
        break;
    case QAbstractSocket::ConnectedState:
        t->DbgPrint("接続状態：Connected");
        onConnected();
        break;
    default:
        break;
    }
}

void MainWindow::onConnected()
{
    setEnableWindow(true);
    ui->connPushButton->setEnabled(true);
    ui->connPushButton->setText("切断");
}

void MainWindow::onDisconnected()
{
    setEnableWindow(false);
    ui->connPushButton->setText("接続");
    ui->remoteLineEdit->setEnabled(true);
    ui->connPushButton->setEnabled(true);
    ui->loginPushButton->setText("ログイン");
    ui->historyListWidget->clear();
}

void MainWindow::onSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    t->DbgPrint(QString("socket error [%1]").arg(m_pWebSocket->errorString()));
}

void MainWindow::onHandshakeError(const QSslError &error)
{
    setEnableWindow(false);
    t->DbgPrint(QString("handshake error (%1)").arg(error.errorString()));
}

void MainWindow::onReceiveMsg(QString message)
{
    qDebug() << Q_FUNC_INFO;
    onReceiveData(message.toUtf8());
}

void MainWindow::onReceiveData(QByteArray message)
{
    t->DbgPrint(message, false);
    QJsonDocument doc = QJsonDocument::fromJson(message);
    if(!doc.isObject()) {
        t->DbgPrint("Json Parser Error!!");
        return;
    }
    QJsonObject o = doc.object();
    QString result = o["result"].toString();
    int id = o["id"].toInt();
    switch(id) {
    case 1: {   // Login
        ui->comboBox->setEnabled(true);
        ui->emailLineEdit->setEnabled(true);
        ui->passLineEdit->setEnabled(true);
        ui->loginPushButton->setText("ログアウト");
        ui->loginPushButton->setEnabled(true);
        t->DbgPrint(o["reason"].toString());
        break; }
    case 2: {   // History List
        if(result == "OK" || result == "Wait") {
            ui->historyListWidget->clear();
            if(o["count"].toInt() > 0) {
                QJsonArray arr = o["history"].toArray();
                for(const auto &a: arr) {
                    QJsonObject obj = a.toObject();
                    QListWidgetItem *item = new QListWidgetItem;
                    item->setData(Qt::UserRole, obj["url"].toString());
                    item->setText(obj["subject"].toString());
                    ui->historyListWidget->addItem(item);
                }
                if(result == "OK") {
                    t->DbgPrint(QString("取得完了(id:%1)").arg(id));
                }else {
                    t->DbgPrint(QString("取得中...(id:%1)").arg(id));
                    break;
                }
            }else {
                t->DbgPrint(QString("データなし(id:%1)").arg(id));
            }
        }else {
            t->DbgPrint(o["reason"].toString());
        }
        ui->historyPushButton->setEnabled(true);
        ui->historyListWidget->setEnabled(true);
        break; }
    case 3: {   // Message
        if(result == "OK" || result == "Wait") {
            QString strHtml;
            QJsonArray arr = o["reply"].toArray();
            for(const auto &v: arr) {
                strHtml += v.toString();
            }
            ui->replyTextEdit->setHtml(strHtml);
            if(result == "OK") {
                t->DbgPrint(QString("取得完了 [model:%1]").arg(o["model"].toString()));
            }else {
                t->DbgPrint(QString("取得中...(id:%1)").arg(id));
                break;
            }
        }else {
            t->DbgPrint(o["reason"].toString());
        }
        ui->requestTextEdit->setText("");
        ui->requestTextEdit->setEnabled(true);
        ui->sendPushButton->setEnabled(true);
        ui->requestTextEdit->setFocus();
        break; }
    case 4: {   // Logout
        ui->comboBox->setEnabled(true);
        ui->emailLineEdit->setEnabled(true);
        ui->passLineEdit->setEnabled(true);
        ui->loginPushButton->setText("ログイン");
        ui->loginPushButton->setEnabled(true);
        ui->historyListWidget->clear();
        t->DbgPrint(o["reason"].toString());
        break; }
    case 5: {   // History Display
        if(result == "OK") {
            QString strHtml;
            QJsonArray arr = o["reply"].toArray();
            for(const auto &a: arr) {
                strHtml += a.toString();
            }
            ui->replyTextEdit->setHtml(strHtml);
            t->DbgPrint(QString("取得完了(id:%1)").arg(id));
        }else {
            t->DbgPrint(o["reason"].toString());
        }
        break; }
    default:
        t->DbgPrint(o["reason"].toString());
        break;
    }
}

void MainWindow::setEnableWindow(bool bEnable)
{
    QList<QWidget *> list = findChildren<QWidget *>();
    for(const auto &l: list) {
        if(l->objectName() != "centralwidget" &&
           l->objectName() != "label_4" &&
           l->objectName() != "remoteLineEdit" &&
           l->objectName() != "connPushButton" &&
           l->objectName() != "statusbar") {
           l->setEnabled(bEnable);
        }
    }
}
