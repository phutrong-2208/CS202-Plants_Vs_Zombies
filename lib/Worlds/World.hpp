#ifndef WORLD_HPP
#define WORLD_HPP

#include <Core/AssetManager.hpp>

#include <Worlds/Grid.hpp>
#include <Worlds/DayMap.hpp>

#include <Gameplay/Plants/Plant.hpp>

class World {
private:
    DayMap dayMap;
    Grid grid;
    PlantFactory plantFactory;
    std::unique_ptr <AssetManager> assetManager = nullptr;
    
    int selectedPlantId;

    // static const char* plantNames[];
    static const int NUM_PLANTS;
    // static const int NUM_ROWS = 5, NUM_COLS = 9;
public:
    World() = default;
    ~World() = default;
    World(int screenWidth, int screenHeight);

    void update(float dt);
    void draw();
    void onMouseClick(Vector2 position);

    int selectedId() const { return selectedPlantId; }
    void selectPlant(int id) { selectedPlantId = id; }
};

#endif
