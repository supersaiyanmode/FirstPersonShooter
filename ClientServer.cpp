#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <cctype>
#include "ClientServer.h"

namespace {
#ifdef WIN32
    int read_(int sock, char* arr, int len){
        return recv(sock, arr,len,0);
    }
    int write_(int sock, const char* arr, int len){
        return send(sock,arr, len, 0);
    }
    int close_(int sock){
        return closesocket(sock);
    }
    WSADATA wsaData;
    int dummy = WSAStartup(MAKEWORD(2,2), &wsaData);
#define socklen_t int
#else
    int read_(int sock, char* arr, int len){
        return ::read(sock, arr,len);
    }
    int write_(int sock, const char* arr, int len){
        return ::write(sock,arr, len);
    }
    int close_(int sock){
        return ::close(sock);
    }
#endif 
    void error(const char *msg){
        perror(msg);
        //exit(1);
    }

    bool readLine(int socket, std::string &ret){
        if (socket == -1)
            return false;
        ret="";
        while (1){
            char cur;
            //std::cout<<"Read till now: "<<ret<<std::endl;
            if (read_(socket,&cur,1)<=0){
                return false;
            }
            if (cur == '\n'){
                return true;
            }
            ret.append(1,cur); //not great, quick and dirty.
        }
    }

    bool writeLine(int socket, const std::string& line){
        if (socket == -1)
            return false;
        std::string str(line);
        int len = line.length();
        if (str[len-1] != '\n')
            str.append(1,'\n');
        return write_(socket,str.c_str(), str.length()) >= 0;
    }
}

//#############SERVER#######################
Server::Server(int p,bool forceUse):acceptThread(*this, &Server::acceptProc,0),
        processThread(*this, &Server::processProc,0), port(p){
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    if (serverSocket < 0) 
        error("ERROR opening socket");
    std::memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    
    int optval = forceUse?1:0;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof optval);
    
    if (bind(serverSocket, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(serverSocket,10);
}

void Server::start(){
    acceptThread.start();
    processThread.start();
}

void Server::acceptProc(int){
    while (serverRunning){
        sockaddr_in client;
        socklen_t clilen = sizeof(client);
        //std::cout<<"Waiting for connection\n";
        int clientSocket = ::accept(serverSocket, (sockaddr*) &client, &clilen);
        if (clientSocket < 0){
            error("ERROR on accept");
        }else{
            std::cout<<"Got one connection\n";
            clients.push_back(ClientStruct());
            int index = clients.size()-1;
            ClientStruct& cs = clients[index];
            std::cout<<clientSocket<<" => "<<index<<std::endl;
            cs.clientSocket = clientSocket;
            cs.inThread = new Thread<Server,int>(*this, &Server::inThreadProc,index);
            cs.outThread = new Thread<Server,int>(*this, &Server::outThreadProc,index);
            cs.inThread->start();
            cs.outThread->start();
        }
    }
}

void Server::processProc(int){
    this->process();
}

void Server::process(){
    //std::cout<<"TOP SERVER PROCESS CALLED!\n";
    while (serverRunning){
        for (int i=0, len=clients.size(); i<len; i++){
            while (clients[i].inQueue.size()){
                std::string s = clients[i].inQueue.front();
                //std::transform(s.begin(),s.end(), s.begin(), ::toupper);
                for (int j=0; j<len; j++){
                    if (i != j)
                        clients[j].outQueue.push(s);
                }
                clients[i].inQueue.pop();
            }
        }
        Thread<Server,int>::sleep(50);
    }
}

void Server::inThreadProc(int index){
    Thread<Server,int>::sleep(std::rand()%100);
    std::cout<<"["<<index<<"] Starting inThreadProc"<<std::endl;
    while (clients[index].clientSocket > 0){
        //std::cout<<"["<<index<<"] Waiting to read a line.\n";
        std::string inLine;
        if (!readLine(clients[index].clientSocket, inLine)){
            //std::cout<<"["<<index<<"] Breaking inThreadProc"<<std::endl;
            break;
        }
        if (inLine != ""){
            //std::cout<<"["<<index<<"] Got line: "<<inLine<<std::endl;
            clients[index].inQueue.push(inLine);
        }
    }
    //std::cout<<"["<<index<<"] Quitting inThreadProc Socket: "<<clients[index].clientSocket<<std::endl;
    clients[index].clientSocket = -1;
}

void Server::outThreadProc(int index){
    Thread<Server,int>::sleep(std::rand()%100);
    std::cout<<"["<<index<<"] Starting outThreadProc"<<std::endl;
    while (clients[index].clientSocket > 0){
        if (!clients[index].outQueue.empty()){
            std::string outLine = clients[index].outQueue.front();
            //std::cout<<"["<<index<<"] Writing: "<<outLine<<std::endl;
            if (!writeLine(clients[index].clientSocket,outLine)){
                //std::cout<<"["<<index<<"] Breaking outThreadProc"<<std::endl;
                break;
            }
            clients[index].outQueue.pop();
        }else{
            Thread<Server,int>::sleep(50);
        }
    }
    //std::cout<<"["<<index<<"] Quitting outThreadProc Socket: "<<clients[index].clientSocket<<std::endl;
    clients[index].clientSocket = -1;
}

Server::~Server(){
    //std::cout<<"Cleaning up clients..\n";
    for (int i=0, len=clients.size(); i<len; i++){
        ::close_(clients[i].clientSocket);
        clients[i].inThread->detach();
        clients[i].outThread->detach();
        delete clients[i].inThread;
        delete clients[i].outThread;
    }
    
    ::close_(serverSocket);
    std::cout<<"Server shut down.\n";
}

//###########CLIENT########################
Client::Client():inThread(*this,&Client::inThreadProc,0),outThread(*this,&Client::outThreadProc,0){
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket<0){
        error("ERROR creating the socket.");
        return;
    }
    std::memset((char*)&serv_addr, 0, sizeof(serv_addr));
}
int Client::connect(std::string address, int p){
    port = p;
    serverAddress = address;
    
    server = gethostbyname(serverAddress.c_str());
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return 0;
    }
    serv_addr.sin_family = AF_INET;
    std::memcpy((char *)&serv_addr.sin_addr.s_addr,(char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    if (::connect(clientSocket,(sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR connecting");
        return 0;
    }
    return 1;
}

void Client::writeLine(const std::string& s){
    outQueue.push(s);
}

std::string Client::requestLine(const std::string& s){
    //WARNING: No reader/writer thread should NOT be active.
    ::writeLine(clientSocket,s);
    std::string str;
    ::readLine(clientSocket,str);
    return str;
}

void Client::lineRead(const std::string& s){
    //std::cout<<"$: "<<s<<std::endl;
}

void Client::start(){
    inThread.start();
    outThread.start();
}

void Client::inThreadProc(int){
    while (true){
        std::string s;
        ::readLine(clientSocket,s);
        if (s != "")
            inQueue.push(s);
    }
}

void Client::outThreadProc(int){
    while (true){
        while (!outQueue.empty()){
            ::writeLine(clientSocket,outQueue.front());
            //std::cout<<"C: "<<outQueue.front();
            outQueue.pop();
        }
        Thread<Client,int>::sleep(50);
    }
}

Client::~Client(){
    inThread.detach();
    outThread.detach();
    ::close_(clientSocket);
}

#if 0
int main(int argc, char *argv[]){
    if (argc < 2){
        std::cout<<"Bad arguments!\n";
        return 1;
    }
    if (!strcmp(argv[1],"server")){
        Server s(51232);
        s.start();
        getchar();
    }else{
        Client client;
        client.start();
        std::string ip;
        std::cout<<"Enter ip address: ";
        std::cin>>ip;
        if (!client.connect(ip.c_str(),51232)){
            error("Unable to connect!");
            return 1;
        }
        std::string string;
        while (string != "quit"){
            std::cout<<"Enter: ";
            std::getline(std::cin,string);
            client.writeLine(string);
        }
    }
    return 0;
}

#endif