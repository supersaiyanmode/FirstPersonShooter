#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <queue>
#include <string>
#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include "Thread.cpp"

class Server{
public:
    Server(int,bool=true);
    ~Server();
    
    void acceptProc(int);
    void processProc(int);
    
    void inThreadProc(int);
    void outThreadProc(int);
    
    void start();
    
    virtual void process();
    
    struct ClientStruct{
        int clientSocket;
        Thread<Server,int> *inThread,*outThread;
        std::queue<std::string> inQueue, outQueue;
        std::string inLine, outLine;
    };
    
protected:
    Thread<Server,int> acceptThread, processThread;
    bool serverRunning;
    int port,serverSocket;
    std::vector<ClientStruct> clients;
};

class Client{
public:
    Client();
    ~Client();
    
    void writeLine(const std::string&);
    std::string requestLine(const std::string&);
    
    virtual void lineRead(const std::string&);
    
    void inThreadProc(int);
    void outThreadProc(int);
    void readerThreadProc(int);
        
    void start();
    
    int connect(std::string, int);
protected:
    int port;
    std::string serverAddress;
    int clientSocket;
    sockaddr_in serv_addr;
    hostent *server;
    Thread<Client,int> inThread,outThread;
    std::queue<std::string> inQueue, outQueue;
    void (*function)(std::string);
};

#endif //SERVER_H 