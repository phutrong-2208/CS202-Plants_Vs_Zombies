#include "Core/TextureManager.hpp"


TextureManager :: TextureManager(){
    texture.clear();
    sound.clear();
    music.clear();
    font.clear();
}
void TextureManager :: LoadResource(void){
    texture["background1"] = LoadTexture("../assets/texture/Maps/Day/background1.jpg");
    if(texture["background1"].id == 0) {
        TraceLog(LOG_ERROR, "Cannot load background1.jpg file");
    }
}   

void TextureManager :: UnloadResource(void){
    UnloadTexture(texture["background1"]);
}   