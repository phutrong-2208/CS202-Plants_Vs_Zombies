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

    // Original PopCap viewport
    float GRID_START_X = 40.0f;  
    float GRID_START_Y = 80.0f;  
    static float CELL_WIDTH[], CELL_HEIGHT[];
    float HORIZONTAL_SCALE, VERTICAL_SCALE;
    float GRID_WIDTH, GRID_HEIGHT;
    const float VIRTUAL_WIDTH = 800.0f, VIRTUAL_HEIGHT = 600.0f;

    int selectedPlantId;

    // static const char* plantNames[];
    static const int NUM_PLANTS;
    // static const int NUM_ROWS = 5, NUM_COLS = 9;

    std::unique_ptr <AssetManager> assetManager = nullptr;
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
