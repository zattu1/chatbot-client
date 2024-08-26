#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    getRegistration();
    connect(ui->emailLineEdit, SIGNAL(editingFinished()), this, SLOT(onLogin()));
    connect(ui->passLineEdit, SIGNAL(editingFinished()), this, SLOT(onPassword()));
    on_comboBox_currentIndexChanged(0);
}

MainWindow::~MainWindow()
{
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


void MainWindow::on_loginPushButton_clicked()
{

}


void MainWindow::on_historyPushButton_clicked()
{

}


void MainWindow::on_sendPushButton_clicked()
{

}

