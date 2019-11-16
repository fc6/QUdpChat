#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

enum MessageType{
    Message,        //消息内容
    Login,       //上线通知
    Logoff,         //下线通知
    FileName,       //
    Refuse          //
};

class QUdpSocket;
class QMenu;
class QAction;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void sendMessage(MessageType msgType,QString srvrAddress="");

    QString getIP();
    QString getIP2();
    QString getUserName();
    QString getSendMessage();
 private slots:
    //接收广播消息并处理槽函数
    void processPendingDatagrams_slot();
    void on_btn_chartClose_clicked();

    void on_btn_chartSend_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_tBtn_setSendBtnShortcut_clicked();

private:
    Ui::MainWindow *ui;

    QUdpSocket      * m_pUdpSocket;
    qint16           m_sPort;
    QMenu           *m_pSendModeMenu;
    QAction         *m_pAction_Send_Enter;
    QAction         *m_pAction_Send_CtrlEnter;

};

#endif // MAINWINDOW_H
