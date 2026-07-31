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

void TexturePackage :: AddMaskedTexture(
    const std :: string& name,
    const std :: string& colorPath,
    const std :: string& maskPath
){
    Image colorImage = LoadImage(colorPath.c_str());
    Image alphaMask = LoadImage(maskPath.c_str());

    if(colorImage.data == nullptr || alphaMask.data == nullptr){
        std :: cerr << "Cannot load masked texture: " << name << '\n';
        if(colorImage.data != nullptr) UnloadImage(colorImage);
        if(alphaMask.data != nullptr) UnloadImage(alphaMask);
        return;
    }

    if(colorImage.width != alphaMask.width ||
       colorImage.height != alphaMask.height){
        std :: cerr << "Texture and mask sizes do not match: " << name << '\n';
        UnloadImage(colorImage);
        UnloadImage(alphaMask);
        return;
    }

    ImageAlphaMask(&colorImage, alphaMask);
    Texture2D texture = LoadTextureFromImage(colorImage);

    UnloadImage(colorImage);
    UnloadImage(alphaMask);

    if(texture.id == 0){
        std :: cerr << "Cannot create masked texture: " << name << '\n';
        return;
    }

    textures[name] = texture;
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
