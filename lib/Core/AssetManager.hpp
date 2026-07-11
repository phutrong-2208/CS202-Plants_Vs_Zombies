#ifndef ASSETMANAGER_HPP
#define ASSETMANAGER_HPP

#include <Core/TextureManager.hpp>
#include <Core/AnimationManager.hpp>

class AssetManager {
private:
    std::unique_ptr <TextureManager> textureManager;
    std::unique_ptr <AnimationManager> animationManager;

    void loadAssets();
public:
    AssetManager();
    TextureManager* getTextureManager() const;
    AnimationManager* getAnimationManager() const;
};

#endif