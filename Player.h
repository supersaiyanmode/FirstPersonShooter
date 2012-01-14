#ifndef PLAYER_H
#define PLAYER_H

#include <algorithm>
#include <vector>
#include "GLCube.h"
#include "Bullet.h"
#include "WorldLoader.h"

class WorldLoader;
class Player: public GLCube{
public:
    Player(WorldLoader*);
    void setDirection(double,double,double);
    void setUp(double,double,double);
    
    void setName(const std::string&);
    std::string getName();
    
    void forward(double);
    void sideStep(double);
    void pitch(double);
    void yaw(double);
    
    void lookAt();
    std::vector<double> getParams();
    void shoot();
    
    virtual std::string notifyString();
    virtual void updateFromString(std::stringstream&);
    
    bool isPointInside(std::vector<double> point);
    
    void paint();
private:
    std::vector<double> prevPosition, prevRotation, prevSize,prevDirection; //notification..
        
    std::vector<double> direction;  //to be used to bullets. Kept in sync with direction
    std::vector<double> up;
    
    WorldLoader* wl;
    std::string name;
};

#endif // PLAYER_H
