#include <Gameplay/Plants/Plant.hpp>


//Shooter Plants
#include "Gameplay/Plants/ShooterPlants/Peashooter.hpp"
#include "Gameplay/Plants/SunProducePlants/Sunflower.hpp"
#include "Gameplay/Plants/DefensivePlants/WallNut.hpp"
#include "Gameplay/Plants/ShooterPlants/SnowPea.hpp"
#include "Gameplay/Plants/ShooterPlants/Repeater.hpp"
#include "Gameplay/Plants/ShooterPlants/Cactus.hpp"


void PlantFactory::setTextureManager(TextureManager* manager) {
    textureManager = manager;
}
void PlantFactory::setAnimationManager(AnimationManager* manager) {
    animationManager = manager;
}

ReanimInstance PlantFactory::createReanim(float scalar, const std::string& packageName, const std::string& animName, const std::string& clipLoopName) {
    ReanimInstance plantReanim;
    plantReanim.setTextureScalar(scalar);
    plantReanim.setTexturePackage(textureManager -> getPackage(packageName));
    plantReanim.setAnimation(animationManager -> getAnimationData(animName));
    plantReanim.playClip(clipLoopName); 

    return plantReanim;
}

std::unique_ptr <Plant> PlantFactory::createPlant(PlantType pType) {
    switch(pType) {
        case SUNFLOWER: {
            std::unique_ptr <Plant> plantPtr = std :: make_unique<Sunflower>();

            plantPtr -> setReanimInstance(
                createReanim(1.5f, "SunFlower", "SunFlowerAnim", "idle")
            );
            
            return plantPtr;
        }
        case PEASHOOTER: {
            std::unique_ptr<Plant> plantPtr = std :: make_unique<Peashooter>();

            plantPtr -> setReanimInstance(
                createReanim(1.5f, "PeaShooter", "PeaShooterSingleAnim", "full_idle")
            );

            return plantPtr;
        } 
        case REPEATER: {
            std::unique_ptr<Plant> plantPtr = std :: make_unique<Repeater>();

            plantPtr -> setReanimInstance(
                createReanim(1.5f, "Repeater", "PeaShooterAnim", "full_idle")
            );

            return plantPtr;
        }
        case SNOWPEA: {
            std::unique_ptr<Plant> plantPtr = std :: make_unique<SnowPea>();

            plantPtr -> setReanimInstance(
                createReanim(1.5f, "SnowPea", "SnowPeaAnim", "full_idle")
            );

            return plantPtr;
        }
        case WALLNUT: {
            std::unique_ptr<Plant> plantPtr = std :: make_unique<WallNut>();

            plantPtr -> setReanimInstance(
                createReanim(1.5f, "WallNut", "WallNutAnim", "idle")
            );

            return plantPtr;
        }
        case CACTUS:{
            std::unique_ptr<Plant> plantPtr = std :: make_unique<Cactus>();
            
            plantPtr -> setReanimInstance(
                createReanim(1.5f, "Cactus", "CactusAnim", "idle")
            );

            return plantPtr;
        }
    }

    return nullptr;
}