#ifndef WORLD_HPP
#define WORLD_HPP

#include <Core/AssetManager.hpp>

#include <Worlds/Grid.hpp>
#include <Worlds/Map.hpp>
#include <Worlds/DayMap.hpp>
#include <Gameplay/Plants/PlantFactory.hpp>


class World {
private:
    std::unique_ptr <Map> map = nullptr;
    Grid grid;
    PlantFactory plantFactory;
    // static const int NUM_ROWS = 5, NUM_COLS = 9;
public:
    World() = default;
    ~World() = default;
    World(int screenWidth, int screenHeight, AssetManager* assetManager);

    void update(float dt);
    void draw();
    void drawPlacementPreview(int selectedPlantId) const;
    bool tryPlacePlant(Vector2 position, PlantType plantType);
    bool isReady() const;
    bool isChoosingPlants() const;
    void finishChoosingPlants();

};

#endif
