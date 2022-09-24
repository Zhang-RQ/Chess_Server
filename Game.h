//
// Created by Zhang on 2021/8/20.
//

#ifndef CHESS_SERVER_GAME_H
#define CHESS_SERVER_GAME_H

#include <cstring>
#include <QByteArray>
#include "mainwindow.h"

class MainWindow;

class Game{
public:
    Game();
    QByteArray EncodeBoard() const;
    void Flip(int x,int y,int player);
    void setReady(int id);
    void setpMainWindow(MainWindow *p);

private:
    bool Ready[2];
    int Board[13][6],Belong[13][6];
    int cntC[2][2];
    MainWindow *pMainWindow;
};

#endif //CHESS_SERVER_GAME_H
