#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include "Bullet.h"
#include "VecDoubleCompare.h"

Bullet::Bullet(Player* player,TextureManager *tm, std::string texPrefix, std::string oid):direction(3,0.0),
        bulletThread(*this,&Bullet::run,0){
    typeString = "bullet";
    
    setTextureManager(tm);
    setTexturePrefix(texPrefix);
    setObjectID(oid);
    setSize(0.01,0.01,0.01);
}

void Bullet::setDirection(double x, double y, double z){
    double sq = std::sqrt(x*x + y*y + z*z);
    direction[0] = x/sq;
    direction[1] = y/sq;
    direction[2] = z/sq;
}

void Bullet::start(){
    bulletThread.start();
}
void Bullet::run(int){
    //std::cout<<"add of moveTransform: "<<moveTransform<<std::endl;
    while (true){
        position[0] += direction[0]/50.0;
        position[1] += direction[1]/50.0;
        position[2] += direction[2]/50.0;
        //std::cout<<"Position: "<<position[0]<<","<<position[1]<<","<<position[2]<<std::endl;
        Thread<Bullet,int>::sleep(10);
    }
}

std::string Bullet::notifyString(){
    if (!vecDoubleEqual(prevPosition,position) || !vecDoubleEqual(prevRotation,rotation)
            || !vecDoubleEqual(prevSize,size) || !vecDoubleEqual(prevDirection,direction)){
        std::stringstream ss;
        
        ss<<std::setprecision(6);
        ss<<     position[0]<<" "<<position[1]<<" "<<position[2];
        ss<<" "<<rotation[0]<<" "<<rotation[1]<<" "<<rotation[2];
        ss<<" "<<size[0]<<" "<<size[1]<<" "<<size[2];
        ss<<" "<<direction[0]<<" "<<direction[1]<<" "<<direction[2];
    
        prevPosition = position;
        prevRotation = rotation;
        prevSize = size;
        prevDirection = direction;
        
        return ss.str();
    }
    return "";
}

void Bullet::updateFromString(std::stringstream& ss){
    ss>>position[0]>>position[1]>>position[2]>>rotation[0]>>rotation[1]>>rotation[2];
    ss>>size[0]>>size[1]>>size[2]>>direction[0]>>direction[1]>>direction[2];
    
    prevPosition = position;
    prevRotation = rotation;
    prevSize = size;
    prevDirection = direction;
}

bool Bullet::isPointInside(std::vector<double>){
    return true;
}

Bullet::~Bullet(){
    bulletThread.detach();
}
void Bullet::paint(){
    GLCube::paint();
}
