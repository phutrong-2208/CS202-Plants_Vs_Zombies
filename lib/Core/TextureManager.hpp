#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <Common.hpp>

class TextureManager{
public:
    std :: map<std :: string, Texture2D> texture;
    std :: map<std :: string, Sound> sound;
    std :: map<std :: string, Music> music;
    std :: map<std :: string, Font> font;
public:
    TextureManager();
    void LoadResource(void);
    void UnloadResource(void);
};

#endif  
