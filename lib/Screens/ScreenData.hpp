#ifndef SCREEN_DATA_HPP
#define SCREEN_DATA_HPP

#include "Common.hpp"
#include "Gameplay/WorldResult.hpp"
#include "Gameplay/Levels/Level.hpp"
#include "Gameplay/Plants/Plant.hpp"

enum class GameMode {ADVENTURE, SURVIVAL_ENDLESS, AI_PVZ};

struct ScreenData{
    WorldResult wResult = WorldResult :: RUNNING;
    LevelID levelID;
    PlantType rewardPlant = PLANT_COUNT;
    GameMode gameMode = GameMode::ADVENTURE;
    int survivalScore = 0;
    int flagsCleared  = 0;
};


#endif
