#ifndef ASSETMANAGER_HPP
#define ASSETMANAGER_HPP

#include <Core/TextureManager.hpp>
#include <Core/AnimationManager.hpp>
#include <Core/TextManager.hpp>
#include <filesystem>

class AssetManager {
private:
    enum class AssetTaskType {
        TEXTURE,
        MASKED_TEXTURE,
        REANIM
    };

    struct AssetLoadTask {
        AssetTaskType type = AssetTaskType::TEXTURE;
        std::string packageName;
        std::string key;
        std::filesystem::path path;
        std::filesystem::path maskPath;
    };

    std::unique_ptr <TextureManager> textureManager;
    std::unique_ptr <AnimationManager> animationManager;
    std::unique_ptr <TextManager> textManager;

    std::vector<AssetLoadTask> loadingQueue;
    std::size_t nextTaskIndex = 0;
    bool loadingStarted = false;
    bool loadingFinished = false;

    void loadBootstrapAssets();
    void prepareLoadingQueue();
    void loadNextTask();
    void loadFonts();

public:
    AssetManager();

    void beginLoading();
    void updateLoading(double timeBudgetMs);
    float getLoadingProgress() const;
    bool isLoadingFinished() const;

    TextureManager* getTextureManager() const;
    AnimationManager* getAnimationManager() const;
    TextManager* getTextManager() const;
};

#endif
