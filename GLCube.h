#ifndef GLCUBE_H
#define GLCUBE_H

#include <string>
#include "GLQuad.h"

class GLCube: public GLObject{
public:
    GLCube();
    virtual ~GLCube();
    virtual void paint();
    bool isPointInside(std::vector<double>);
    
    std::string notifyString();
    void updateFromString(std::stringstream&);
    
    std::vector<double> dimensions;
private:
    std::vector<double> prevPosition, prevRotation, prevSize;
};

#endif // GLCUBE_H
