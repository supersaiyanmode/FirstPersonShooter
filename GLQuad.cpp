#include "GLQuad.h"
#include <GL/gl.h>

GLQuad::GLQuad(){}

GLQuad::~GLQuad(){}

std::string GLQuad::notifyString(){
    return "";
}

void GLQuad::paint(){
    startTransform();
    glEnable(GL_TEXTURE_2D);
    getTextureManager()->bindTextureID(getTexturePrefix() + "_face");
    glBegin(GL_QUADS);
        glTexCoord2i(0,0);  glVertex2d(-1.0,-1.0);
        glTexCoord2i(0,1);  glVertex2d(-1.0,1.0);
        glTexCoord2i(1,1);  glVertex2d(1.0,1.0);
        glTexCoord2i(1,0);  glVertex2d(1.0,-1.0);
    glEnd();
    endTransform();
}

bool GLQuad::isPointInside(std::vector<double>){
    return false;
}
