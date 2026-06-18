#include "Core/TextureManager.hpp"

void TextureManager :: addPackage(const std :: string& name, std :: unique_ptr<TexturePackage> package){
    packs[name] = std :: move(package);
}
TexturePackage* TextureManager :: getPackage(const std :: string& name){
    auto it = packs.find(name);
    if(it != packs.end()){
        return it -> second.get();
    }
    else{
        return nullptr;
    }
}