#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include "raylib.h"
#include "unordered_map"
#include "string"

class TextureManager{
public:
    std :: unordered_map<std :: string, Texture2D> texture;
    std :: unordered_map<std :: string, Sound> sound;
    std :: unordered_map<std :: string, Music> music;
    std :: unordered_map<std :: string, Font> font;
public:
    TextureManager();
    void LoadResource(void);
    void UnloadResource(void);
};

extern TextureManager file;

#endif  
