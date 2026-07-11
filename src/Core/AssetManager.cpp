#include <Core/AssetManager.hpp>

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

                if(fileName.find(".png") != std :: string :: npos) {
                    fileName.erase(fileName.find(".png"), 4);
                    for(char& c : fileName) {
                        if(islower(c)) c = toupper(c);
                    }
                    Entity -> AddTexture(fileName, entry.path().string());  
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
    TraceLog(LOG_INFO, "All assets loaded successfully !");
}
AssetManager::AssetManager() {
    textureManager = std::make_unique <TextureManager> ();
    animationManager = std::make_unique <AnimationManager> ();
    loadAssets();
}

TextureManager* AssetManager::getTextureManager() const {
    return textureManager.get();
}
AnimationManager* AssetManager::getAnimationManager() const {
    return animationManager.get();
}