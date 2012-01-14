#ifndef GAMECLIENT_H
#define GAMECLIENT_H
#include <vector>
#include <algorithm>
#include <queue>
#include "Thread.cpp"
#include "ClientServer.h"
#include "Player.h"
#include "WorldLoader.h"

class GameClient:public Client{
public:
    GameClient();
    ~GameClient();
    
    int mouseMove(int, int);
    //int mouseClick(int,int,int,int);
    void processKeys();
    
    bool connect(const std::string&);
    void initialise(const std::string&);

    void paint();
    void mainloop();
    
    void readerThreadProc(int);
    void notifyChanges();
    
    void lineRead(std::string); //overriden
private:
    bool gameRunning;
    int windowWidth, windowHeight;
    bool keysState[256];
    
    std::string mapName,playerName;
    
    Player *mainPlayer;
    std::vector<Player*> players;
    WorldLoader wl;
    Thread<GameClient,int> readerThread;
};

#endif //GAMECLIENT_H