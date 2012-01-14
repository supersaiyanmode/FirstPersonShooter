#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H
#include<map>
#include<string>
#include<GL/gl.h>
#include<algorithm>

class TextureManager
{
    public:
        TextureManager();
        virtual ~TextureManager();
        void addTexture(std::string, std::string);
        bool IDExists(std::string);
        GLuint getTextureFromID(std::string);
        void bindTextureID(std::string);
        void clear();
    protected:
    private:
        std::map<std::string,GLuint> textures;
        std::map<std::string,GLuint> file2idMap;
        bool glTextureEnabled;
};

#endif // TEXTUREMANAGER_H
