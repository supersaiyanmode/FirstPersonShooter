#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "WorldLoader.h"

#include "GLCube.h"
#include <GL/glu.h>
#include "Bullet.h"


double parseDouble(std::string s){
    std::stringstream ss(s);
    double d;
    ss>>d;
    return d;
} 
std::string getNewRandomOID(){
    static int init = 0;
    static std::vector<std::string> Ids;
    if (!init++)
        std::srand((unsigned)time(0));
    bool unique = false;
    std::string id;
    while (!unique){
        std::stringstream ss;
        ss<<rand();
        id = ss.str();
        unique = std::find(Ids.begin(), Ids.end(), id)==Ids.end();
    }
    Ids.push_back(id);
    return id;
}

WorldLoader::WorldLoader():serverMode(false){
}

void WorldLoader::readFile(std::string s){
    worldFileName = s;
    objects.clear();
    tm.clear();
    std::stringstream ss;
    ss<<std::ifstream(s.c_str()).rdbuf();
    readString(ss.str());
}

void WorldLoader::readString(std::string s){
    jsonRoot = parseJSON(s);
    JSONValue& jo = *jsonRoot;
    
    if (!serverMode){
        //Read and load the textures
        std::cout<<"Reading textures\n";
        std::vector<std::string> textures = jo["textures"].keys();
        
        for (std::vector<std::string>::iterator it=textures.begin(); it!=textures.end(); it++){
            //std::cout<<*it<<std::endl;
            tm.addTexture(*jo["textures"][*it],*it);
        }
    }
    //Read Projection details..
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(parseDouble(*jo["projection"]["fov"]),
                    parseDouble(*jo["projection"]["aspectRatio"]),
                    parseDouble(*jo["projection"]["near"]),
                    parseDouble(*jo["projection"]["far"])
    );

    //std::cout<<jo.stringise()<<std::endl;

    //load objects..
    int objectsCount = jo["objects"].length();
    //std::cout<<"Loading "<<objectsCount<<" objects."<<std::endl;
    for (int i=0; i<objectsCount; i++){
        GLObject* glo;
        if (*jo["objects"][i]["type"] == "cube"){
            glo = new GLCube();
            glo->setTextureManager(&tm);
            glo->setTexturePrefix(*jo["objects"][i]["texturePrefix"]);
            if (serverMode){
                jo["objects"][i].setMapValue("oid",parseJSON("\"" + getNewRandomOID() + "\""));
            }
            glo->setObjectID(*jo["objects"][i]["oid"]);
            
            
            //set up transformations..
            double position[3], rotation[3], size[3];
            std::istringstream  issp(*jo["objects"][i]["position"]),
                                isso(*jo["objects"][i]["orientation"]),
                                isss(*jo["objects"][i]["dimensions"]);
            
            issp>>position[0]>>position[1]>>position[2];
            isso>>rotation[0]>>rotation[1]>>rotation[2];
            isss>>size[0]>>size[1]>>size[2];
            glo->setPosition(position[0], position[1], position[2]);
            glo->setRotation(rotation[0], rotation[1], rotation[2]);
            glo->setSize(size[0], size[1], size[2]);
        }else{
            std::cout<<"WHAT?!?!?!"<<std::endl;
            exit(2);
        }
        addObject(glo);
    }
    std::cout<<"Loaded "<<objectsCount<<" objects\n";
}

void WorldLoader::setServerMode(bool b){
    serverMode = b;
}

std::string WorldLoader::getMapString(){
    return jsonRoot->stringise(false);
}

void WorldLoader::addObject(GLObject* glo){
    //std::cout<<"Object loaded: "<<glo->getName()<<std::endl;
    objects.push_back(glo);
}

TextureManager* WorldLoader::getTextureManager(){
    return &tm;
}

std::string WorldLoader::notifyString(){
    std::string s="";
    for (std::vector<GLObject*>::iterator it=objects.begin(); it!=objects.end(); it++){
        std::string ns = (*it)->notifyString();
        if (ns != "")
            s += "/object " + (*it)->type() + " " + (*it)->getObjectID() + " " + ns + "\n";
    }
    return s;
}

void WorldLoader::updateFromString(std::stringstream& ss){
    std::string type,id;
    ss>>type>>id;
    for (std::vector<GLObject*>::iterator it=objects.begin(); it!=objects.end(); it++){
        if (/*type == "cube" && */id==(*it)->getObjectID()){ //will hold for even bullets
            (*it)->updateFromString(ss);
            return;
        }
    }
    //object not found, create a new one!
    if (type == "cube"){
        GLCube* glc = new GLCube();
        glc->setTextureManager(&tm);
        glc->setTexturePrefix("cube_box");
        glc->updateFromString(ss);
        objects.push_back(glc);
        std::cout<<"Added a new object: "<<glc->getObjectID()<<std::endl;
    }
}

WorldLoader::~WorldLoader(){
    for (std::vector<GLObject*>::iterator it=objects.begin(); it!=objects.end(); it++){
        delete *it;
    }
}

void WorldLoader::paint(){
    for (std::vector<GLObject*>::iterator it=objects.begin(); it!=objects.end(); it++){
        (*it)->paint();
    }
}
