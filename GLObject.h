#ifndef GLOBJECT_H
#define GLOBJECT_H
#include <string>
#include <vector>
#include <sstream>

#include "TextureManager.h"

class GLObject{
public:
    GLObject();
    virtual ~GLObject();
    virtual void paint()=0;
    
    void setTextureManager(TextureManager*);
    TextureManager* getTextureManager();
    
    void setTexturePrefix(std::string);
    std::string getTexturePrefix();
    
    void setObjectID(const std::string&);
    std::string getObjectID();
    
    std::string type();
    
    virtual std::string notifyString()=0;
    virtual void updateFromString(std::stringstream&)=0;
    
    virtual bool isPointInside(std::vector<double>);

    void setPosition(double,double,double);
    void setRotation(double, double, double);
    void setSize(double, double, double);
    
protected:
    void startTransform();
    void endTransform();
    std::vector<double> position, rotation, size;
    std::vector<double> velocity, acceleration;
    std::string typeString;
private:
    std::string objectID;
    std::string texturePrefix;
    TextureManager* tm;
};

#endif // GLOBJECT_H
