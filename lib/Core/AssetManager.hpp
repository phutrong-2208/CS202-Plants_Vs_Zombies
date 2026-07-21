#ifndef ASSETMANAGER_HPP
#define ASSETMANAGER_HPP

#include <Core/TextureManager.hpp>
#include <Core/AnimationManager.hpp>
#include <Core/TextManager.hpp>

class AssetManager {
private:
    std::unique_ptr <TextureManager> textureManager;
    std::unique_ptr <AnimationManager> animationManager;
    std::unique_ptr <TextManager> textManager;

    void loadAssets();
    void loadFonts();
public:
    AssetManager();
    TextureManager* getTextureManager() const;
    AnimationManager* getAnimationManager() const;
    TextManager* getTextManager() const;
};

#endif
