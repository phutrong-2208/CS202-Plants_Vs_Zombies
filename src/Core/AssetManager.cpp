#include <Core/AssetManager.hpp>
#include <chrono>
#include <filesystem>

namespace {
    std :: string toUpperKey(std :: string key) {
        for(char& c : key) {
            if(islower(c)) c = toupper(c);
        }
        return key;
    }

    bool isTextureExtension(const std::string& extension) {
        return extension == ".PNG" ||
               extension == ".JPG" ||
               extension == ".JPEG";
    }

    std::filesystem::path findMaskPath(const std::filesystem::path& texturePath) {
        const std::string stem = texturePath.stem().string();
        const auto maskPath = texturePath.parent_path() / (stem + "_.png");
        return std::filesystem::exists(maskPath) ? maskPath : std::filesystem::path{};
    }

    bool isAlphaMaskFile(const std::filesystem::path& path) {
        const std::string extension = toUpperKey(path.extension().string());
        const std::string stem = path.stem().string();
        if (extension != ".PNG" || stem.empty() || stem.back() != '_') {
            return false;
        }

        const std::string colorStem = stem.substr(0, stem.size() - 1);
        return std::filesystem::exists(path.parent_path() / (colorStem + ".jpg")) ||
               std::filesystem::exists(path.parent_path() / (colorStem + ".jpeg"));
    }
}

void AssetManager::loadBootstrapAssets() {
    const std::filesystem::path loadMenuDir =
        std::filesystem::path(PROJECT_DIR) / "assets/texture/Misc/LoadMenu";

    if (!std::filesystem::exists(loadMenuDir)) {
        TraceLog(LOG_ERROR, "AssetManager: LoadMenu bootstrap assets are missing");
        loadFonts();
        return;
    }

    auto package = std::make_unique<TexturePackage>();
    for (const auto& entry : std::filesystem::recursive_directory_iterator(loadMenuDir)) {
        if (entry.is_directory()) continue;

        const std::filesystem::path path = entry.path();
        const std::string extension = toUpperKey(path.extension().string());
        const std::string stem = path.stem().string();

        if (isTextureExtension(extension)) {
            if (isAlphaMaskFile(path)) continue;

            const std::string key = toUpperKey(stem);
            const std::filesystem::path maskPath = findMaskPath(path);
            if ((extension == ".JPG" || extension == ".JPEG") && !maskPath.empty()) {
                package->AddMaskedTexture(key, path.string(), maskPath.string());
            } else {
                package->AddTexture(key, path.string());
            }
            continue;
        }

        if (extension == ".REANIM") {
            auto parser = std::make_unique<ReanimParser>();
            if (parser->loadFromFile(path.string())) {
                animationManager->addAnimationData(stem + "Anim", std::move(parser));
            }
        }
    }

    textureManager->addPackage("LoadMenu", std::move(package));
    loadFonts();
}

void AssetManager::prepareLoadingQueue() {
    loadingQueue.clear();
    nextTaskIndex = 0;

    const std::filesystem::path assetDir =
        std::filesystem::path(PROJECT_DIR) / "assets/texture";
    if (!std::filesystem::exists(assetDir)) {
        loadingFinished = true;
        return;
    }

    for (const auto& category : std::filesystem::directory_iterator(assetDir)) {
        if (!category.is_directory()) continue;

        for (const auto& entity : std::filesystem::directory_iterator(category.path())) {
            if (!entity.is_directory()) continue;

            const std::string entityName = entity.path().filename().string();
            if (entityName == "LoadMenu") continue;

            textureManager->addPackage(entityName, std::make_unique<TexturePackage>());

            for (const auto& entry : std::filesystem::recursive_directory_iterator(entity.path())) {
                if (entry.is_directory()) continue;

                const std::filesystem::path path = entry.path();
                const std::string extension = toUpperKey(path.extension().string());
                const std::string stem = path.stem().string();

                if (isTextureExtension(extension)) {
                    if (isAlphaMaskFile(path)) continue;

                    AssetLoadTask task;
                    task.packageName = entityName;
                    task.key = toUpperKey(stem);
                    task.path = path;

                    const std::filesystem::path maskPath = findMaskPath(path);
                    if ((extension == ".JPG" || extension == ".JPEG") && !maskPath.empty()) {
                        task.type = AssetTaskType::MASKED_TEXTURE;
                        task.maskPath = maskPath;
                    } else {
                        task.type = AssetTaskType::TEXTURE;
                    }
                    loadingQueue.push_back(std::move(task));
                    continue;
                }

                if (extension == ".REANIM") {
                    AssetLoadTask task;
                    task.type = AssetTaskType::REANIM;
                    task.key = stem + "Anim";
                    task.path = path;
                    loadingQueue.push_back(std::move(task));
                }
            }
        }
    }

    loadingFinished = loadingQueue.empty();
}

void AssetManager::loadNextTask() {
    if (nextTaskIndex >= loadingQueue.size()) {
        loadingFinished = true;
        return;
    }

    const AssetLoadTask& task = loadingQueue[nextTaskIndex++];
    if (task.type == AssetTaskType::REANIM) {
        auto parser = std::make_unique<ReanimParser>();
        if (parser->loadFromFile(task.path.string())) {
            animationManager->addAnimationData(task.key, std::move(parser));
        }
    } else {
        TexturePackage* package = textureManager->getPackage(task.packageName);
        if (package != nullptr) {
            if (task.type == AssetTaskType::MASKED_TEXTURE) {
                package->AddMaskedTexture(
                    task.key,
                    task.path.string(),
                    task.maskPath.string()
                );
            } else {
                package->AddTexture(task.key, task.path.string());
            }
        }
    }

    if (nextTaskIndex >= loadingQueue.size()) {
        loadingFinished = true;
        TraceLog(LOG_INFO, "All assets loaded successfully!");
    }
}

void AssetManager::loadFonts() {
    std :: filesystem :: path fontDir = std :: filesystem :: path(PROJECT_DIR) / "assets/font";
    if (!std :: filesystem :: exists(fontDir)) {
        return;
    }

    for (const auto& entry : std :: filesystem :: recursive_directory_iterator(fontDir)) {
        if (entry.is_directory()) continue;

        std :: string extension = toUpperKey(entry.path().extension().string());
        if (extension != ".TTF" && extension != ".OTF") {
            continue;
        }

        std :: string stemKey = toUpperKey(entry.path().stem().string());
        std :: string folderKey = toUpperKey(entry.path().parent_path().filename().string());

        textManager -> addFont(stemKey, entry.path().string());
        textManager -> addAlias(folderKey, stemKey);
    }
}

AssetManager::AssetManager() {
    textureManager = std::make_unique <TextureManager> ();
    animationManager = std::make_unique <AnimationManager> ();
    textManager = std::make_unique <TextManager> ();
}

void AssetManager::beginLoading() {
    if (loadingStarted) return;

    loadBootstrapAssets();
    prepareLoadingQueue();
    loadingStarted = true;
}

void AssetManager::updateLoading(double timeBudgetMs) {
    if (!loadingStarted || loadingFinished) return;

    const auto startTime = std::chrono::steady_clock::now();
    do {
        loadNextTask();
        const auto elapsed = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - startTime
        ).count();
        if (elapsed >= timeBudgetMs) break;
    } while (!loadingFinished);
}

float AssetManager::getLoadingProgress() const {
    if (!loadingStarted) return 0.0f;
    if (loadingQueue.empty()) return 1.0f;

    return static_cast<float>(nextTaskIndex) /
           static_cast<float>(loadingQueue.size());
}

bool AssetManager::isLoadingFinished() const {
    return loadingStarted && loadingFinished;
}

TextureManager* AssetManager::getTextureManager() const {
    return textureManager.get();
}
AnimationManager* AssetManager::getAnimationManager() const {
    return animationManager.get();
}
TextManager* AssetManager::getTextManager() const {
    return textManager.get();
}
