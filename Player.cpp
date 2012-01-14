#include <GL/glu.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Player.h"
#include "VecDoubleCompare.h"

void vectorNormalise(std::vector<double>& v){
    double length = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    for (int i=0; i<3; i++)
        v[i]/=length;
}

void crossProduct(const std::vector<double>& x,const std::vector<double>& y,std::vector<double>& res){
    res[0] = x[1]*y[2] - x[2]*y[1];
    res[1] = x[2]*y[0] - x[0]*y[2];
    res[2] = x[0]*y[1] - x[1]*y[0];
    vectorNormalise(res);
}

Player::Player(WorldLoader* w):prevPosition(3,0.0), prevRotation(3,0.0), prevSize(3,0.0),
                                     prevDirection(3,0.0), direction(3,0.0),up(3,0.0),wl(w){
    typeString = "player";
    
    position[0] = 0.0;
    position[1] = 0.0;
    position[2] = 5.0;
    
    direction[0] = 0.0;
    direction[1] = 0.0;
    direction[2] = -1.0;
    
    up[0] = 0.0;
    up[1] = 1.0;
    up[2] = 0.0;
    
}

void Player::setDirection(double x,double y,double z){
    direction[0] = x;
    direction[1] = y;
    direction[2] = z;
    vectorNormalise(direction);
}
void Player::setUp(double x,double y,double z){
    up[0] = x;
    up[1] = y;
    up[2] = z;
}

void Player::setName(const std::string& s){
    name = s;
}

std::string Player::getName(){
    return name;
}

void Player::forward(double val){
    position[0] += val * direction[0];
    position[1] += val * direction[1];
    position[2] += val * direction[2];
}
void Player::sideStep(double val){
    std::vector<double> yaw(3,0.0);
    
    crossProduct(up,direction,yaw);
    
    position[0] += val * yaw[0];
    position[1] += val * yaw[1];
    position[2] += val * yaw[2];
}

void Player::pitch(double val){
    double x = direction[0], y =direction[2];
    
    direction[1] += val/100.0;
    rotation[0] = std::atan2(direction[1], std::sqrt(x*x + y*y))*180.0/3.14159;
}
void Player::yaw(double val){
    double x = direction[0], y =direction[2];
    double r = std::sqrt(x*x + y*y), theta = std::atan2(y,x);
    theta += val/40.0;
    direction[0] = r*std::cos(theta);
    direction[2] = r*std::sin(theta);
    vectorNormalise(direction);
    
    //convert to degrees, opengl degrees..
    //minus crap adjustment
    rotation[1] = -theta*180.0/3.14159;
}

void Player::lookAt(){
    gluLookAt(position[0],position[1],position[2],
              position[0]+direction[0],position[1]+direction[1],position[2]+direction[2],
              up[0], up[1], up[2]);
}

std::vector<double> Player::getParams(){
    std::vector<double> v;
    v.insert(v.end(), position.begin(), position.end());
    v.insert(v.end(), direction.begin(), direction.end());
    v.insert(v.end(), up.begin(), up.end());
    return v;
}

std::string Player::notifyString(){
    //notify player's location
    if (!vecDoubleEqual(prevPosition,position) || !vecDoubleEqual(prevRotation,rotation)
            || !vecDoubleEqual(prevSize,size) || !vecDoubleEqual(prevDirection,direction)){
        std::stringstream ss;
        ss<<name;
        ss<<std::setprecision(6);
        ss<<" "<<position[0]<<" "<<position[1]<<" "<<position[2];
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

void Player::updateFromString(std::stringstream& ss){
    std::string str;
    ss>>str;
    setName(str);
    ss>>position[0]>>position[1]>>position[2]>>rotation[0]>>rotation[1]>>rotation[2];
    ss>>size[0]>>size[1]>>size[2]>>direction[0]>>direction[1]>>direction[2];
    prevPosition = position;
    prevRotation = rotation;
    prevSize = size;
    prevDirection = direction;
}

void Player::shoot(){
    Bullet *bullet = new Bullet(this,getTextureManager(),"cube_box","bullet");
    bullet->setPosition(position[0], position[1], position[2]);
    bullet->setDirection(direction[0], direction[1], direction[2]);
    bullet->start();
    wl->addObject(bullet);
}

bool Player::isPointInside(std::vector<double> point){
    return false;
}

void Player::paint(){
    GLCube::paint();
}
