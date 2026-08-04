#ifndef PLANTFACTORY_HPP
#define PLANTFACTORY_HPP

#include <Gameplay/Plants/Plant.hpp>
#include <functional>

class PlantDataset {
private:
    std::map <PlantType, std::unique_ptr<PlantData>> plantDatasetMap;
    void loadFromFile(const std::string& filepath);
public:
    PlantDataset();
    PlantData* getPlantData(PlantType pType);
};

class PlantFactory {
private:
    TextureManager* textureManager;
    AnimationManager* animationManager;

    std::unique_ptr <PlantDataset> plantDataStorage = nullptr;

    using PlantCreator = std::function<std::unique_ptr<Plant>()>;
    std::map <PlantType, PlantCreator> creatorMap;
    
    ReanimInstance createReanim(float scalar, const std::string& packageName, const std::string& animName, const std::string& clipLoopName);
public:
    void setTextureManager(TextureManager* manager);
    void setAnimationManager(AnimationManager* manager);

    void loadPlantMechanics();
    std::unique_ptr <Plant> createPlant(PlantType pType);
    int getSunCost(PlantType pType) const;
    std::map<PlantType, int> getAllSunCosts() const;
};

#endif