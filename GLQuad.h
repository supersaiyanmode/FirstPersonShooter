#ifndef GLQUAD_H
#define GLQUAD_H
#include "GLObject.h"
#include <string>
class GLQuad: public GLObject{
public:
    GLQuad();
    virtual ~GLQuad();
    virtual void paint();
    virtual std::string notifyString();
    bool isPointInside(std::vector<double>);
};

#endif // GLQUAD_H
