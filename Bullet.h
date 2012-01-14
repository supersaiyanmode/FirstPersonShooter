#ifndef BULLET_H
#define BULLET_H
#include <string>
#include <vector>

#include "GLCube.h"
#include "WorldLoader.h"
#include "Player.h"
#include "Thread.cpp"

class WorldLoader;
class TextureManager;
class Player;
class Bullet: public GLCube{
public:
    Bullet(Player*, TextureManager *, std::string, std::string);
    ~Bullet();
    void setDirection(double, double, double);
    
    std::string notifyString();
    void updateFromString(std::stringstream&);
    
    bool isPointInside(std::vector<double>);
    
    void start();
    void run(int);
    
    void paint();
private:
    std::vector<double> direction;
    Thread<Bullet,int> bulletThread;
    std::vector<double> prevPosition, prevRotation, prevSize,prevDirection;
};

#endif // BULLET_H
