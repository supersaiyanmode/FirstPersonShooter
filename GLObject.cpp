#include <GL/gl.h>
#include "GLObject.h"

GLObject::GLObject(){
    position.assign(3,0.0);
    rotation.assign(3,0.0);
    size.assign(3,1.0);
}
GLObject::~GLObject(){

}
void GLObject::paint(){

}

void GLObject::setTextureManager(TextureManager* t){
    tm = t;
}
TextureManager* GLObject::getTextureManager(){
    return tm;
}


std::string GLObject::type(){
    return typeString;
}
void GLObject::setObjectID(const std::string& s){
    objectID = s;
}
std::string GLObject::getObjectID(){
    return objectID;
}

void GLObject::setTexturePrefix(std::string p){
    texturePrefix = p;
}
std::string GLObject::getTexturePrefix(){
    return texturePrefix;
}

bool GLObject::isPointInside(std::vector<double>){
    return false;
}

void GLObject::setPosition(double x, double y, double z){
    position[0] = x;
    position[1] = y;
    position[2] = z;
}
void GLObject::setRotation(double x, double y, double z){
    rotation[0] = x;
    rotation[1] = y;
    rotation[2] = z;
}
void GLObject::setSize(double x, double y, double z){
    size[0] = x;
    size[1] = y;
    size[2] = z;
}


void GLObject::startTransform(){
    glPushMatrix();
    glTranslated(position[0], position[1], position[2]);

    glRotated(rotation[0], 1.0,0.0,0.0);
    glRotated(rotation[1], 0.0,1.0,0.0);
    glRotated(rotation[2], 0.0,0.0,1.0);

    glScaled(size[0], size[1], size[2]);
}
void GLObject::endTransform(){
    glPopMatrix();
}
