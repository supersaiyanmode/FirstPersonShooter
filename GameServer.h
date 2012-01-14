#ifndef GAMESERVER_H
#define GAMESERVER_H
#include <vector>
#include <algorithm>
#include <queue>

#include "ClientServer.h"
#include "WorldLoader.h"

class GameServer:public Server{
public:
    GameServer();
    ~GameServer();
    
    void start();
    
    std::pair<std::vector<int>, std::string> analyseString(const std::string&,int);
    void process(int);
    
    bool active();
    void stop();

    void setMap(const std::string&);
private:
    struct GameClientStruct{
        Player* player;
        std::string name;
        int points;
        bool ready;
    };
    std::map<int, GameClientStruct> players;
    
    std::vector<int> allClients(int=-1);
    Thread<GameServer,int> acceptThread, processThread;
    std::string mapFile;
    std::string mapString;
    WorldLoader wl;
};

#endif //GAMESERVER_H