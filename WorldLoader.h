#ifndef WORLDLOADER_H
#define WORLDLOADER_H

#include <vector>
#include <string>

#include "TextureManager.h"
#include "GLObject.h"
#include "JSON.h"

class Player;
class WorldLoader{
public:
    WorldLoader();
    virtual ~WorldLoader();
    void readFile(std::string);
    void readString(std::string);
    void addObject(GLObject*);
    TextureManager* getTextureManager();
    void setServerMode(bool);
    std::string getMapString();
    void paint();
    void updateFromString(std::stringstream&);
    std::string notifyString();
protected:
private:
    bool serverMode;
    JSONValue *jsonRoot;
    std::vector<GLObject*> objects;
    TextureManager tm;
    std::string worldFileName;
    
};

std::string getNewRandomOID();
#endif // WORLDLOADER_H
