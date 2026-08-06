#ifndef SCREEN_DATA_HPP
#define SCREEN_DATA_HPP

#include "Common.hpp"
#include "Gameplay/WorldResult.hpp"
#include "Gameplay/Levels/Level.hpp"
#include "Gameplay/Plants/Plant.hpp"

struct ScreenData{
    WorldResult wResult = WorldResult :: RUNNING;
    LevelID levelID;
    PlantType rewardPlant = PLANT_COUNT;
};


#endif