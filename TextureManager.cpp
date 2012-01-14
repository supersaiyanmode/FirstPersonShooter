#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <GL/glu.h>
#include <GL/glfw.h>
#include "soil/SOIL.h"
#include "TextureManager.h"
#include "GLQuad.h"

bool fileExists(std::string fileName){
    FILE* img = NULL;
    img = fopen(fileName.c_str(),"rb");
    if (img) return fclose(img),true;
    return false;
}
GLuint loadImageFromFileAsTexture(std::string s){
    //std::cout<<"Loading: "<<s<<std::endl;
    if (!fileExists(s)){
        std::cout<<"Image file: "<<s<<" doesn't exist!"<<std::endl;
        exit(1);
    }
    GLuint texture = SOIL_load_OGL_texture(s.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
        );
    glBindTexture( GL_TEXTURE_2D, texture ); //bind the texture to it's array
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    return texture;
}


TextureManager::~TextureManager(){
    //need to unbind glTextures here..
}

TextureManager::TextureManager():glTextureEnabled(false){
    
}

void TextureManager::clear(){
    textures.clear();
    file2idMap.clear();
}
void TextureManager::addTexture(std::string fileName, std::string _id){
    if (!glTextureEnabled){
        glTextureEnabled = true;
        glEnable(GL_TEXTURE_2D);
    }
    
    if (IDExists(_id)){
        std::cout<<"ID already exists: "<<_id<<std::endl;
        exit(1);
    }
    
    //implement File look up, prevent duplicat filename textures..
    if (file2idMap.find(fileName) != file2idMap.end()){
        textures[_id] =  file2idMap[fileName];
        //std::cout<<_id<<"=>"<<fileName<<"=>"<<textures[_id];
        //std::cout<<" [Image already loaded: "<<fileName<<"]"<<std::endl;
        return;
    }
    if (!fileExists(fileName)){
        std::cout<<"File doesn't exist: "<<fileName<<std::endl;
        exit(1);
    }
    
    GLuint temp = loadImageFromFileAsTexture(fileName);
    //std::cout<<_id<<"=>"<<fileName<<"=>"<<temp<<std::endl;
    textures[_id] = temp;
    file2idMap[fileName] = temp;
}

bool TextureManager::IDExists(std::string _id){
    return textures.find(_id) != textures.end();
}

GLuint TextureManager::getTextureFromID(std::string _id){
    if(!_id.compare("NONE"))
        return 0;
    if (!IDExists(_id)){
        std::cout<<"Texture ID: "<<_id<<" doesnt exist!"<<std::endl;
        exit(1);
    }
    return (*textures.find(_id)).second;
}

void TextureManager::bindTextureID(std::string _id){
    if (!this || _id == "NONE") //when objects dont have a proper reference to TextureManager
        glBindTexture(GL_TEXTURE_2D,0);
    else
        glBindTexture(GL_TEXTURE_2D,getTextureFromID(_id));
    //std::cout<<"balh!\n";
}

#if 0
#include "GLQuad.cpp"
#include "GLObject.cpp"

GLQuad glq;
void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glq.paint();
    glutSwapBuffers();
    glutPostRedisplay();
}
int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(500,500);
    glutCreateWindow("FPS!");
    
    TextureManager tm;
    tm.addTexture("images/BoxTexture.bmp","quad_face");
    
    glq.setTexturePrefix("quad");
    
    glutDisplayFunc(display);
    
    glutMainLoop();
}
#endif
