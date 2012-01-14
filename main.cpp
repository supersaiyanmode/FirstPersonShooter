#include <math.h>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include "GameServer.h"
#include "GameClient.h"
#include "Thread.h"

int main(int argc, char **argv) {
    if (argc > 1 && std::string(argv[1]) == "server"){
        GameServer gs;
        gs.setMap("world.json");
        gs.start();

        Thread<GameClient,int>::sleep(1);
        GameClient gc;
        gc.connect("localhost");
        std::cout<<"Creating client.\n";
        gc.initialise("server_player!");
        
        gc.start();
        gc.mainloop();

        std::cin>>argc;
        gs.stop();
    }else{
        std::string ip;
        GameClient gc;
        do{
            std::cout<<"Enter IP Address to connect to: ";
            std::cin>>ip;
        }while(!gc.connect(ip));
        
        gc.initialise("Player_client");
        
        gc.start();
        gc.mainloop();
    }

    return 0;
}
