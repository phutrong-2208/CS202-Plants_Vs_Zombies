#ifndef PLANT_AI_CONTROLLER_HPP
#define PLANT_AI_CONTROLLER_HPP

#include <Common.hpp>
#include <Gameplay/Plants/PlantType.hpp>

class World;

class PlantAIController {
private:
    World* world = nullptr;
    float decisionTimer = 0.0f;
    float elapsedTime = 0.0f;
    std :: map<PlantType, float> rechargeTimes;
    std :: map<PlantType, int> sunCosts;
    std :: map<PlantType, float> cooldownRemaining;
    std :: array<std::array<PlantType, 9>, 5> previousPlants{};
    std :: array<int, 5> retreatLevel{};
    bool plantSnapshotInitialized = false;

    bool tryEmergencyDefense();
    bool tryGrowEconomy();
    bool trySpendSurplusSun();
    bool tryPlace(int lane, int column, PlantType type);
    bool tryPlaceWithReserve(int lane, int column, PlantType type, int reserve);
    bool tryPlaceCloserToHouse(int lane, int normalColumn, PlantType type);
    bool tryPlaceCloserToHouseWithReserve(int lane, int normalColumn, PlantType type, int reserve);
    bool tryPlaceNearColumn(int lane, int targetColumn, PlantType type);
    bool isCoolingDown(PlantType type) const;
    int countPlants(PlantType type) const;
    bool hasPlantTypeInLane(int lane, PlantType type) const;
    int countPlantTypeInLane(int lane, PlantType type) const;
    float getLaneDefenseScore(int lane) const;
    void updatePlantLossMemory();

public:
    void setWorld(World* targetWorld);
    void reset();
    void update(float dt);
};

#endif
