#ifndef ASSETMANAGER_HPP
#define ASSETMANAGER_HPP

#include <Core/TextureManager.hpp>
#include <Core/AnimationManager.hpp>
#include <Core/TextManager.hpp>
#include <Core/SoundManager.hpp>
#include <Core/MusicManager.hpp>
#include <filesystem>

class AssetManager {
private:
    enum class AssetTaskType {
        TEXTURE,
        MASKED_TEXTURE,
        REANIM,
        SOUND,
        MUSIC
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
    std::unique_ptr <SoundManager> soundManager;
    std::unique_ptr <MusicManager> musicManager;

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
    void updateAudio();
    float getLoadingProgress() const;
    bool isLoadingFinished() const;

    TextureManager* getTextureManager() const;
    AnimationManager* getAnimationManager() const;
    TextManager* getTextManager() const;
    SoundManager* getSoundManager() const;
    MusicManager* getMusicManager() const;
};

#endif
