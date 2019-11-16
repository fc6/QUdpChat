#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QUdpSocket>
#include <QHostInfo>
#include<QProcess>
#include <QScrollBar>
#include<QDateTime>
#include <QNetworkInterface>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pUdpSocket = new QUdpSocket(this);
    m_sPort =quint16(67890);
    QAbstractSocket::BindMode  bindmode = QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint;
    m_pUdpSocket->bind(m_sPort,bindmode);
  //  m_pUdpSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption,1);
    connect(m_pUdpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams_slot()));

    sendMessage(Login,"");
   // ui->btn_chartSend->setShortcut( QKeySequence("Ctrl+Return")); //Ctrl+Return √
    //ui->btn_chartSend->setShortcut( QKeySequence(tr("Return"))); //Return
   // ui->btn_chartSend->setShortcut( QKeySequence(Qt::Key_Return)); //Ctrl+Return
    m_pSendModeMenu = new QMenu(ui->tBtn_setSendBtnShortcut);

    QAction * m_pAction_Send_Enter=m_pSendModeMenu->addAction(QStringLiteral("按Enter发送消息"));
    m_pAction_Send_Enter->setCheckable(true);
    QAction * m_pAction_Send_CtrlEnter=m_pSendModeMenu->addAction(QStringLiteral("按Ctrl+Enter发送消息"));
    m_pAction_Send_CtrlEnter->setCheckable(true);
    m_pAction_Send_CtrlEnter->setChecked(true);

}

MainWindow::~MainWindow()
{
    sendMessage(Logoff);
    delete ui;
}

void MainWindow::sendMessage(MessageType msgType, QString srvrAddress)
{
    QByteArray  msgdata;
    QDataStream sendMsgStream(&msgdata,QIODevice::WriteOnly);

    QString localHostName = QHostInfo::localHostName();
    QString localIp = getIP2();
    sendMsgStream<<msgType<<getUserName()<<localHostName<<localIp;
    switch(msgType)
    {
    case Message:
        if(ui->textEdit->toPlainText()=="")
            return;
        else {
            sendMsgStream<<getSendMessage();
            ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
        }
        break;
    case Login:    //上线通知
        break;
      case  Logoff:         //下线通知
       break;
    case  FileName:         //下线通知
     break;
    case  Refuse:         //下线通知
     break;
    default:
    break;

    }
    //UDP广播通知
    m_pUdpSocket->writeDatagram(msgdata,QHostAddress::Broadcast,m_sPort);

}
QString MainWindow::getIP()
{
    QString ip=NULL;
    QHostInfo hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
    foreach (QHostAddress address, hostInfo.addresses()) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            ip=address.toString();
            break;
        }
    }
    return ip;
}

QString MainWindow::getIP2()
{
    QString ip=NULL;
    QList<QHostAddress> list= QNetworkInterface::allAddresses();
    foreach (QHostAddress address,list) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            ip=address.toString();
            break;
        }
    }
    return ip;
}

QString MainWindow::getUserName()
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment=QProcess::systemEnvironment();
    foreach(QString string,envVariables)
    {
        int index=environment.indexOf(QRegExp(string));
        if(index!=-1)
        {
            QStringList stringlList=environment.at(index).split('=');
            if(stringlList.size()==2)
            {
                return stringlList.at(1);
                break;
            }
        }
    }
    return "unknown";
}

QString MainWindow::getSendMessage()
{
    QString msg=ui->textEdit->toHtml();
    ui->textEdit->clear();
    ui->textEdit->setFocus();
    return msg;
}

void MainWindow::processPendingDatagrams_slot()
{
     QByteArray msgData;
    //有待处理数据就读出来显示
    while(m_pUdpSocket->hasPendingDatagrams())
    {
       QString ip =m_pUdpSocket->peerAddress().toString();
        msgData.resize(m_pUdpSocket->pendingDatagramSize());
       m_pUdpSocket->readDatagram(msgData.data(),msgData.size());
       QDataStream getDataStream(&msgData,QIODevice::ReadOnly);
       int msgType;
       QString userName,localHostName,ipAddress,message;
       QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
       getDataStream>>msgType;
       switch(msgType)
       {
       case Message:
            getDataStream>>userName>>localHostName>>ipAddress>>message;
            ui->textBrowser ->setTextColor(Qt::blue);
            ui->textBrowser->setCurrentFont(QFont("黑体",12));
            ui->textBrowser->append("["+userName+"]"+time);
            ui->textBrowser->append(message);
           break;
       case Login:
             getDataStream>>userName>>localHostName>>ipAddress;
             //有人上线需要处理
             if(ipAddress!=getIP2())
             {
                 ui->textBrowser ->setTextColor(Qt::green);
                 ui->textBrowser->setCurrentFont(QFont("宋体",10));
                 ui->textBrowser->append(QString("%1 上线了！").arg(userName));
             }


           break;
       case Logoff:
            getDataStream>>userName>>localHostName;
            //有人下线需要处理
            ui->textBrowser ->setTextColor(Qt::gray);
            ui->textBrowser->setCurrentFont(QFont("宋体",10));
            ui->textBrowser->append(QString("%1 下线了！").arg(userName));

           break;
       default:
           break;
       }
    }
}

void MainWindow::on_btn_chartClose_clicked()
{
    this->close();
}

void MainWindow::on_btn_chartSend_clicked()
{
    sendMessage(Message);
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->textEdit->setFontPointSize(arg1.toDouble());
    ui->textEdit->setFocus();
}

void MainWindow::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->textEdit->setCurrentFont(f);
    ui->textEdit->setFocus();
}


void MainWindow::on_tBtn_setSendBtnShortcut_clicked()
{
    QPoint p= QCursor::pos();//ui->tBtn_setSendBtnShortcut->pos();
    m_pSendModeMenu->exec(p);
}
