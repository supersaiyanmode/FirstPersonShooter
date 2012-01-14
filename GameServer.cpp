#include <iostream>
#include <fstream>
#include <sstream>
#include "GameServer.h"
#include "Player.h"
//#############GAME SERVER##################
GameServer::GameServer():Server(51232),acceptThread(*this,&GameServer::acceptProc,0),
            processThread(*this, &GameServer::process,0){

}

void GameServer::start(){
    std::cout<<"Reading Map: "<<mapFile<<std::endl;
    wl.setServerMode(true);
    try{
        wl.readFile(mapFile);
    }catch(char const* s){
        std::cout<<"Exception: "<<s<<std::endl;
        exit(-1);
    }
    mapString = wl.getMapString();
    std::cout<<"Map string length: "<<mapString.length()<<std::endl;
    serverRunning = true;
    acceptThread.start();
    processThread.start();

    std::cout<<"Game Server started."<<std::endl;
}

std::vector<int> GameServer::allClients(int except){
    std::vector<int> v;
    for (int i=0,len=Server::clients.size(); i<len; i++)
        if (players[i].ready && (except == -1 || except != i))
            v.push_back(i);
    return v;
}

std::pair<std::vector<int>, std::string> GameServer::analyseString(const std::string& s, int index){
    std::cout<<"["<<index<<"] Analysing: "<<s<<std::endl;
    std::stringstream ss(s);
    std::string type;
    ss>>type;
    if (type == "/init"){
        std::string str;
        ss>>str;
        if (str == "map"){
            return std::make_pair(std::vector<int>(1,index),mapString);
        }else if (str == "main_player_config"){
            std::string name;
            ss>>name;
            players[index] = GameClientStruct();
            GameClientStruct &gcs = players[index];
            gcs.ready = true;
            gcs.player = new Player(&wl);
            gcs.player->setObjectID(getNewRandomOID());
            gcs.player->setName(name);
            return std::make_pair(allClients(),"/object player " + gcs.player->getObjectID() + " " +
                                        gcs.player->notifyString());
        }else if (str == "ready"){
            players[index].ready = true;
            return std::make_pair(std::vector<int>(1,index),"ACK");
        }
    }else if(type == "/object"){
        std::string str;
        ss>>str;
        if (str == "player"){
            Player *p = players[index].player;
            ss>>str;
            if (str != p->getObjectID()){
                std::cout<<"Bad player OID!\n";
                return std::make_pair(std::vector<int>(),"");
            }
            players[index].player->updateFromString(ss);
            return std::make_pair(allClients(index),s);
        }else if (str == "cube"){
            //transfer to WL..
        }
    }else{
        //return std::make_pair(allClients(),s);
    }
    return std::make_pair(std::vector<int>(),"");
}

void GameServer::process(int){
    std::cout<<"GameServer::Process() called\n";
    while (serverRunning){
        for (int i=0, len=Server::clients.size(); i<len; i++){
            while (Server::clients[i].inQueue.size()){
                std::string s = Server::clients[i].inQueue.front();
                Server::clients[i].inQueue.pop();
                std::pair<std::vector<int>,std::string> res = analyseString(s,i);
                std::vector<int>& toVector = res.first;
                s = res.second;
                if (s == "")
                    continue;
                for (std::vector<int>::iterator it=toVector.begin(); it!=toVector.end(); it++){
                    std::cout<<"["<<*it<<"] Writing line: "<<s<<std::endl;
                    Server::clients[*it].outQueue.push(s);
                }
            }
        }
        Thread<Server,int>::sleep(50);
    }
}

void GameServer::stop(){
    serverRunning = false;
    acceptThread.detach();
    //processThread.detach();
}

bool GameServer::active(){
    return serverRunning;
}

void GameServer::setMap(const std::string& m){
    mapFile = m;
}

GameServer::~GameServer(){
    for (std::map<int, GameClientStruct>::iterator it=players.begin(); it!=players.end(); it++)
        delete it->second.player;
    std::cout<<"Game Server Destroyed."<<std::endl;
}
