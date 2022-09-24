//
// Created by Zhang on 2021/8/20.
//

#include "Game.h"

const static std::pair<int,int> HQs[]={std::make_pair(3,2),std::make_pair(3,4),
                                       std::make_pair(4,3),std::make_pair(5,2),std::make_pair(5,4),
                                       std::make_pair(8,2),std::make_pair(8,4),
                                       std::make_pair(9,3),std::make_pair(10,2),std::make_pair(10,4)};

Game::Game()
{
    Ready[0]=Ready[1]=false;
    memset(cntC,0,sizeof cntC);
    memset(Board,0,sizeof Board);
    memset(Belong,-1,sizeof Belong);
    std::vector<std::pair<int,int>> V;
    srand(time(0));
    for(int i=0;i<3;i++)
        V.emplace_back(rand(),0),
        V.emplace_back(rand(),1),
        V.emplace_back(rand(),2),
        V.emplace_back(rand(),3),
        V.emplace_back(rand(),4),
        V.emplace_back(rand(),5),
        V.emplace_back(rand(),18),
        V.emplace_back(rand(),19); //16
    for(int i=0;i<2;i++)
        V.emplace_back(rand(),6),
        V.emplace_back(rand(),7),
        V.emplace_back(rand(),8),
        V.emplace_back(rand(),9),
        V.emplace_back(rand(),10),
        V.emplace_back(rand(),11),
        V.emplace_back(rand(),12),
        V.emplace_back(rand(),13),
        V.emplace_back(rand(),20),
        V.emplace_back(rand(),21); //20
    V.emplace_back(rand(),14);
    V.emplace_back(rand(),15);
    V.emplace_back(rand(),16);
    V.emplace_back(rand(),17);
    V.emplace_back(rand(),22);
    V.emplace_back(rand(),23);
    std::sort(V.begin(),V.end());
    for(int x=1;x<=12;x++)
        for(int y=1;y<=5;y++)
        {
            std::pair<int,int> T=std::make_pair(x,y);
            bool ok=true;
            for(const std::pair<int,int>& HQ:HQs)
                ok&=T!=HQ;
            if(!ok)
                continue;
            Q_ASSERT(!V.empty());
            Board[x][y]=V.back().second;
            V.pop_back();
        }
    for(const std::pair<int,int>& HQ:HQs)
        Board[HQ.first][HQ.second]=24;
}

QByteArray Game::EncodeBoard() const
{
    QByteArray msg;
    msg.append((char)5);
    for(int x=1;x<=12;x++)
        for(int y=1;y<=5;y++)
            msg.append((char)(Board[x][y]<<1|(Board[x][y]!=24))); //Not Hidden
    return msg;
}

void Game::Flip(int x, int y, int player)
{
    qDebug("x=%d y=%d Belong=%d",x,y,Belong[x][y]);
    Q_ASSERT(Belong[x][y]==-1);
    Belong[x][y]=player;
    if(++cntC[player][Board[x][y]&1]==2)
        pMainWindow->ColorSelected(player,Board[x][y]&1);
}

void Game::setReady(int id)
{
    Ready[id]=true;
    if(Ready[0]&&Ready[1])
        pMainWindow->StartGame();
}

void Game::setpMainWindow(MainWindow *p)
{
    pMainWindow=p;
}