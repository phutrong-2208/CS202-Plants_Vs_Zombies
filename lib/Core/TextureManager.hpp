#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <Common.hpp>
#include "TexturePackage.hpp"

class TextureManager{
private:
    std :: unordered_map<std :: string, std :: unique_ptr<TexturePackage>> packs;
public:
    void addPackage(const std :: string& name, std :: unique_ptr<TexturePackage> package);
    TexturePackage* getPackage(const std :: string& name);
};

#endif  
