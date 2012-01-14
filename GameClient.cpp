#include <iostream>
#include <sstream>
#include <iomanip>
#include <GL/glfw.h>
#include <cmath>
#include "GameClient.h"

GameClient* gameClientPtr; //inited to zero. TODO: Check NULL on usage.
static const int windowWidth=400, windowHeight=400;
bool dummyBool;

void mouseMovement(int x, int y){
    gameClientPtr->mouseMove(x - windowWidth/2, windowHeight/2 - y);
    glfwSetMousePos(windowWidth/2,windowHeight/2);
}
//##############GAME CLIENT#################
GameClient::GameClient():Client(),readerThread(*this, &GameClient::readerThreadProc,0){
    gameClientPtr = this;
    glfwInit();
    Thread<Server,int>::sleep(1);
    GLFWvidmode dvm;
    glfwGetDesktopMode(&dvm);
    glfwSetWindowTitle("FPS!");
    glfwOpenWindow(400, 400, dvm.RedBits, dvm.GreenBits, dvm.BlueBits,
                        8, 24, 0, GLFW_WINDOW);
    glEnable(GL_DEPTH_TEST);
    
    for (int i=0; i<256; keysState[i++]=0);
}

bool GameClient::connect(const std::string& ip){
    return Client::connect(ip.c_str(),51232);
}

void GameClient::initialise(const std::string& nm){
    std::cout<<"Loading...\n";
    std::string map = requestLine("/init map");
    std::cout<<"Contacted Server, loading map..\n";
    wl.readString(map);
    readerThread.start();
    
    //Loading player!
    mainPlayer = new Player(&wl);
    mainPlayer->setTextureManager(wl.getTextureManager());
    mainPlayer->setTexturePrefix("cube_box");
    mainPlayer->setName(nm);
    std::stringstream ss;
    ss<<requestLine("/init main_player_config " + nm);
    std::string str;
    ss>>str;
    if (str != "/object")
        std::cout<<"Bad response from the server: "<<str<<" "<<ss.str()<<std::endl;
    ss>>str;
    if (str != "player")
        std::cout<<"Bad response from the server: /object"<<str<<" "<<ss.str()<<std::endl;
    
    ss>>str;
    mainPlayer->setObjectID(str);
    mainPlayer->updateFromString(ss);
    std::cout<<"Main player loaded!\n";
    
    str = requestLine("/init ready");
    if (str != "ACK"){
        std::cout<<"Error. Bad response from the server!\n";
        ::exit(1);
    }
    //attach mouse..
    void mouseMovement(int,int);
    glfwSetMousePosCallback(mouseMovement);
}

void GameClient::mainloop(){
    gameRunning = true;
    int notifyCounter = 0;
    while (gameRunning){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        paint();
        glfwSwapBuffers();

        //processing keys for next paint..
        processKeys();
        gameRunning = glfwGetWindowParam(GLFW_OPENED);

        notifyCounter = (notifyCounter+1)%30;
        if (!notifyCounter){
            notifyChanges();
        }
        Thread<GameClient,int>::sleep(20);
    }
    glfwTerminate();
}


int GameClient::mouseMove(int deltaX, int deltaY){
    mainPlayer->yaw(deltaX/10.0);
    mainPlayer->pitch(deltaY/10.0);
    return true;
}
/*int GameClient::mouseClick(int button, int state, int x, int y){
    if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
        mainPlayer->shoot();
    return true;        //move the pointer to center of the screen..
}*/

void GameClient::processKeys(){
#define keyDown(X) (glfwGetKey(X)==GLFW_PRESS)
    if (keyDown('A'))     mainPlayer->sideStep(0.1);
    if (keyDown('D'))     mainPlayer->sideStep(-0.1);
    if (keyDown('W'))     mainPlayer->forward(0.1);
    if (keyDown('S'))     mainPlayer->forward(-0.1);
    if (keyDown('Q'))     gameRunning = false;
#undef keyDown

    if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS)
        mainPlayer->shoot();
}

void GameClient::paint(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    processKeys();
    mainPlayer->lookAt();
    //gluLookAt(0,10,15, 0,0,0, 0,1,0);


    wl.paint();
    //dont paint mainplayer, paint all others..
    for (std::vector<Player*>::iterator it=players.begin(); it!=players.end(); it++){
        (*it)->paint();
    }
}


void GameClient::readerThreadProc(int){
    while(true){
        while (!inQueue.empty()){
            this->lineRead(inQueue.front());
            inQueue.pop();
        }
        Thread<Client,int>::sleep(50);
    }
}

void GameClient::notifyChanges(){
    std::string str,changes = wl.notifyString();
    str = mainPlayer->notifyString();
    if (str != "")
        changes += "/object player " + mainPlayer->getObjectID() + " " +str + "\n";
    if (changes.length())
        //std::cout<<"NOTIFY CHANGES: "<<changes<<std::endl;
        outQueue.push(changes);
}
void GameClient::lineRead(std::string s){
    std::cout<<"Accepting change: "<<s<<std::endl;
    std::stringstream ss;
    ss<<s;
    std::string str;
    ss>>str;
    
    if (str == "/object"){
        ss>>str;
        if (str == "player"){
            ss>>str; //str is now the ObjectID
            for (std::vector<Player*>::iterator it=players.begin(); it!=players.end(); it++){
                if ((*it)->getObjectID() == str){
                    (*it)->updateFromString(ss);
                    return;
                }
            }
            if (str == mainPlayer->getObjectID()){ //random crap
                std::cout<<"WHAT?!?!?!?\n";
                exit(0);
            }
            Player* p = new Player(&wl);
            p->setTextureManager(wl.getTextureManager());
            p->setTexturePrefix("cube_box");
            p->setObjectID(str);
            p->updateFromString(ss);
            players.push_back(p);
            std::cout<<"New player connected: "<<p->getName()<<std::endl;
        }else{
            wl.updateFromString(ss);
        }
    }
}

GameClient::~GameClient(){
    delete mainPlayer;
    for(std::vector<Player*>::iterator it=players.begin(); it!=players.end(); it++)
        delete *it;
    readerThread.detach();
}
