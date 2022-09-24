//
// Created by Zhang on 2021/8/20.
//

#ifndef CHESS_SERVER_MAINWINDOW_H
#define CHESS_SERVER_MAINWINDOW_H

#include <QWidget>
#include <QtNetwork/QtNetwork>
#include "Game.h"

const char EndSign=(char)0xFF;

class Game;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void ColorSelected(int player,int color);
    void StartGame();
    void HeartbeatTimeout(int id);

private:
    int Clients;
    Ui::MainWindow *ui;
    QTcpServer *pTServer;
    QTcpSocket *pTSocket[3];
    void HandleNewTransmission(int id);
    Game *pGame;
    QTimer *pTimer10s[3];
    time_t LastHeartbeatTime[3];
    bool Connected[3];

public slots:
    void HandleNewConnection();
    void Restart();
};


#endif //CHESS_SERVER_MAINWINDOW_H
