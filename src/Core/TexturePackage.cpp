#include "Core/TexturePackage.hpp"

void TexturePackage :: AddTexture(const std :: string& name, const std :: string& path){
    Texture2D tex = LoadTexture(path.c_str());
    if(tex.id == 0){
        std :: cerr << "Cannot load textures: " << name << '\n';
        return;
    }
    else{   
        textures[name] = tex;
    }
}

Texture2D* TexturePackage :: GetTexture(const std :: string &name){
    auto it = textures.find(name);

    if(it != textures.end()){
        return &it -> second;
    }
    else{
        return nullptr;
    }
}