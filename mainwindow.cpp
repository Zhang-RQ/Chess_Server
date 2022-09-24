//
// Created by Zhang on 2021/8/20.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QString ServerIP;
    QList<QHostAddress> IPs=QNetworkInterface::allAddresses();
    for(const QHostAddress& IP:IPs)
        if(IP!=QHostAddress::LocalHost&&IP.toIPv4Address())
            ServerIP=IP.toString();
    ui->label_2->setText(ServerIP);
    ui->label_5->setText("Not Connected");
    ui->label_6->setText("Not Connected");

    pTServer=new QTcpServer(this);
    pTServer->listen(QHostAddress::Any,23333);
    connect(pTServer,&QTcpServer::newConnection,this,&MainWindow::HandleNewConnection);

    pGame=new Game;Clients=0;
    pGame->setpMainWindow(this);

    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::Restart);

    LastHeartbeatTime[0]=LastHeartbeatTime[1]=0;

    Connected[0]=Connected[1]=false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::HandleNewConnection()
{
    qDebug("New Connection");
    int nowClient=0;
    if(Connected[0])
        nowClient=1;

    pTSocket[nowClient]=pTServer->nextPendingConnection();
    pTSocket[nowClient]->setSocketOption(QAbstractSocket::LowDelayOption,1);
    if(nowClient==0)
        connect(pTSocket[nowClient],&QTcpSocket::readyRead,[=](){HandleNewTransmission(0);});
    else
        connect(pTSocket[nowClient],&QTcpSocket::readyRead,[=](){HandleNewTransmission(1);});
    QByteArray msg;
    msg.append((char)0);
    msg.append(EndSign);
    pTSocket[nowClient]->write(msg);
    msg=pGame->EncodeBoard();
    msg.append(EndSign);
    pTSocket[nowClient]->write(msg);

    pTimer10s[nowClient]=new QTimer;
    pTimer10s[nowClient]->setInterval(10000);
    pTimer10s[nowClient]->start();
    LastHeartbeatTime[nowClient]=time(0);

    if(nowClient==0)
    {
        Connected[0]=true;
        ui->label_5->setText("Connected");
        connect(pTSocket[nowClient],&QTcpSocket::disconnected,[&](){this->ui->label_5->setText("Disconnected");Connected[0]=false;});
        connect(pTimer10s[nowClient],&QTimer::timeout,[&](){
            //qDebug("now=%lld last=%lld",time(0),LastHeartbeatTime[0]);
            if(time(0)-LastHeartbeatTime[0]>10)
                pTSocket[0]->close();
        });
    }
    if(nowClient==1)
    {
        Connected[1]=true;
        ui->label_6->setText("Connected");
        connect(pTSocket[nowClient],&QTcpSocket::disconnected,[&](){this->ui->label_6->setText("Disconnected");Connected[1]=false;});
        connect(pTimer10s[nowClient],&QTimer::timeout,[&](){
            if(time(0)-LastHeartbeatTime[1]>10)
                pTSocket[1]->close();
        });
    }

    Clients++;
}

void MainWindow::HandleNewTransmission(int id)
{
    QByteArray msgAll=pTSocket[id]->readAll();
    QList<QByteArray> V=msgAll.split(EndSign);
    qDebug("Handle Transmission len=%d package num=%d id=%d",msgAll.length(),V.length(),id);
    for(const QByteArray& msg:V)
    {
        if(msg.length()==0)
            continue;
        int Sign=(int)msg.constData()[0];
        switch(Sign)
        {
            case 1:qDebug("HandShake with %d",id);break;
            case 3:pGame->setReady(id);(id?ui->label_6:ui->label_5)->setText("Ready");break;
            case 5: case 6:pTSocket[!id]->write(msg+EndSign);break;
            case 7:pGame->Flip(msg.constData()[1],msg.constData()[2],id);break;
            case 9:LastHeartbeatTime[id]=time(0);break;
            default:Q_ASSERT(0);
        }
    }
}

void MainWindow::ColorSelected(int player, int color)
{
    qDebug("Color Selected player=%d",player);    QByteArray msg1,msg2;
    msg1.append((char)4);msg2.append((char)4);
    msg1.append((char)color);msg2.append((char)(color^1));
    msg1.append((char)1);msg2.append((char)0);
    msg1.append(EndSign);msg2.append(EndSign);
    pTSocket[player]->write(msg1);
    pTSocket[!player]->write(msg2);
    ui->label_7->setText("Game in Progress");
}

void MainWindow::StartGame()
{
    qDebug("Start Game");
    qDebug()<<(unsigned long long)this;
    ui->label_7->setText("Distributing Color");
    QByteArray msg1,msg2;
    msg1.append((char)2);msg1.append((char)1);msg1.append(EndSign);
    msg2.append((char)2);msg2.append((char)0);msg2.append(EndSign);
    pTSocket[0]->write(msg1);
    pTSocket[1]->write(msg2);
}

void MainWindow::Restart()
{
    qApp->exit(998244353);
}

void MainWindow::HeartbeatTimeout(int id)
{
    pTSocket[id]->close();
}