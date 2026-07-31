#include <Core/AssetManager.hpp>
#include <filesystem>

namespace {
    std :: string toUpperKey(std :: string key) {
        for(char& c : key) {
            if(islower(c)) c = toupper(c);
        }
        return key;
    }
}

void AssetManager::loadAssets() {
    std :: string assetDir = std :: string (PROJECT_DIR) + "assets/texture";
    for (const auto& category : std :: filesystem :: directory_iterator(assetDir)){ 
        
        if(!category.is_directory()){
            continue;
        }
        //iterator Zombies, Plants, Projectiles, Maps, ...

        for(const auto& entity : std :: filesystem :: directory_iterator(category.path())){
            if(!entity.is_directory()) continue;
            std :: string entityName = entity.path().filename().string();

            std :: unique_ptr<TexturePackage> Entity = std :: make_unique<TexturePackage>();
            bool hasTexture = false;
            for (const auto& entry : std :: filesystem:: recursive_directory_iterator(entity.path())){
                if(entry.is_directory()) continue;

                std :: string fileName = entry.path().filename().string();
                const std :: string extension =
                    toUpperKey(entry.path().extension().string());

                if(extension == ".PNG" ||
                   extension == ".JPG" ||
                   extension == ".JPEG") {
                    const std :: filesystem :: path texturePath = entry.path();
                    const std :: string stem = texturePath.stem().string();

                    // Files named Foo_.png are alpha masks for Foo.jpg.
                    // They must not be registered as independent textures.
                    if(extension == ".PNG" &&
                       !stem.empty() &&
                       stem.back() == '_') {
                        const std :: string colorStem =
                            stem.substr(0, stem.size() - 1);
                        const auto jpgPath =
                            texturePath.parent_path() / (colorStem + ".jpg");
                        const auto jpegPath =
                            texturePath.parent_path() / (colorStem + ".jpeg");

                        if(std :: filesystem :: exists(jpgPath) ||
                           std :: filesystem :: exists(jpegPath)) {
                            continue;
                        }
                    }

                    const std :: string textureKey = toUpperKey(stem);

                    if(extension == ".JPG" || extension == ".JPEG") {
                        const auto maskPath =
                            texturePath.parent_path() / (stem + "_.png");

                        if(std :: filesystem :: exists(maskPath)) {
                            Entity -> AddMaskedTexture(
                                textureKey,
                                texturePath.string(),
                                maskPath.string()
                            );
                            hasTexture = true;
                            continue;
                        }
                    }

                    Entity -> AddTexture(textureKey, texturePath.string());
                    hasTexture = true;
                }

                if(fileName.find(".reanim") != std :: string :: npos){
                    auto parser = std :: make_unique<ReanimParser>();
                    parser -> loadFromFile(entry.path().string());

                    // Use the filename stem (not the folder name) as the key.
                    // This distinguishes PeaShooter.reanim ("PeaShooterAnim") from
                    // PeaShooterSingle.reanim ("PeaShooterSingleAnim") which live in
                    // the same entity folder and would otherwise overwrite each other.
                    std :: string stem = fileName.substr(0, fileName.find(".reanim"));
                    std :: string dataName = stem + "Anim";
                    animationManager -> addAnimationData(dataName, std :: move(parser));
                }
            }
            if (hasTexture) {
                textureManager -> addPackage(entityName, std::move(Entity));
            }
        }
    }
    loadFonts();
    TraceLog(LOG_INFO, "All assets loaded successfully !");
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
    loadAssets();
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
