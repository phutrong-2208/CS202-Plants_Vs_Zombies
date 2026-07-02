#ifndef TEXTURE_PACKAGE_HPP
#define TEXTURE_PACKAGE_HPP

#include "Common.hpp"

class TexturePackage{
private:
    std :: unordered_map<std :: string, Texture2D> textures;

public: 
    ~TexturePackage(){
        for(auto&x : textures){
            UnloadTexture(x.second);
        }
    }

    void AddTexture(const std :: string& name, const std :: string& path);
    Texture2D* GetTexture(const std :: string &name);
};  


#endif