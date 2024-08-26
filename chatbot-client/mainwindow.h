#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

#define MAX_RESERVE 10
#define EXENAME "chatbot-client"

class CPersonalInformation {
public:
    CPersonalInformation() {}
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

private:
    void setRegistration();
    void getRegistration();

private slots:
    void on_comboBox_currentIndexChanged(int index);
    void onEmail();
    void onPassword();

    void on_loginPushButton_clicked();

    void on_historyPushButton_clicked();

    void on_sendPushButton_clicked();

private:
    Ui::MainWindow *ui;
    CPersonalInformation m_personalInformation[MAX_RESERVE];
};
#endif // MAINWINDOW_H
