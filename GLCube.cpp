#include <GL/gl.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "GLCube.h"
#include "VecDoubleCompare.h"

const GLfloat n[6][3]= {  /* Normals for the 6 faces of a cube. */
    {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
    {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0}
};
const GLint faces[6][4] = {/* Vertex indices for the 6 faces of cube. */
    {0, 1, 2, 3}, {3, 2, 6, 7}, {5, 4, 7, 6},
    {4, 5, 1, 0}, {1, 5, 6, 2}, {4, 0, 3, 7}
};

#if 0                          
                          
     4        5           
                          
  0       1                

                          
     7        6           
                          
  3       2               
                          
#endif
const GLfloat v[8][3] = {
    {-1,-1,1},{-1,-1,-1},{-1,1,-1},{-1,1,1},
    {1,-1,1},{1,-1,-1},{1,1,-1},{1,1,1}
};
GLCube::GLCube(){
    typeString = "cube";
}
GLCube::~GLCube(){}

void GLCube::paint(){
    std::string faceStr[6] = {"left","top","right","bottom","back","front"};
    startTransform();
        for (int i = 0; i < 6; i++) {
            getTextureManager()->bindTextureID(getTexturePrefix()+"_"+faceStr[i]);
            glBegin(GL_QUADS);
                glNormal3fv(&n[i][0]);
                glTexCoord2f(0.0,1.0); glVertex3fv(&v[faces[i][0]][0]);
                glTexCoord2f(1.0,1.0); glVertex3fv(&v[faces[i][1]][0]);
                glTexCoord2f(1.0,0.0); glVertex3fv(&v[faces[i][2]][0]);
                glTexCoord2f(0.0,0.0); glVertex3fv(&v[faces[i][3]][0]);
            glEnd();
        }
    endTransform();
}

std::string GLCube::notifyString(){
    //notify player's location
    if (!vecDoubleEqual(prevPosition,position) || !vecDoubleEqual(prevRotation,rotation)
            || !vecDoubleEqual(prevSize,size)){
        std::stringstream ss;
        ss<<std::setprecision(6);
        ss<<     position[0]<<" "<<position[1]<<" "<<position[2];
        ss<<" "<<rotation[0]<<" "<<rotation[1]<<" "<<rotation[2];
        ss<<" "<<size[0]<<" "<<size[1]<<" "<<size[2];
        prevPosition = position;
        prevRotation = rotation;
        prevSize = size;
        
        return ss.str();
    }
    return "";
}

void GLCube::updateFromString(std::stringstream& ss){
    ss>>position[0]>>position[1]>>position[2]>>rotation[0]>>rotation[1]>>rotation[2];
    ss>>size[0]>>size[1]>>size[2];
    prevPosition = position;
    prevRotation = rotation;
    prevSize = size;
}


bool GLCube::isPointInside(std::vector<double>){
    return true;
}
