#ifndef WORLD_HPP
#define WORLD_HPP

#include <Core/TextureManager.hpp>  
#include <Core/AnimationManager.hpp>

#include <Worlds/Grid.hpp>
#include <Worlds/DayMap.hpp>

#include "Gameplay/Plants/ShooterPlants/Peashooter.hpp"
#include "Gameplay/Plants/SunProducePlants/Sunflower.hpp"
#include "Gameplay/Plants/DefensivePlants/WallNut.hpp"
#include "Gameplay/Plants/ShooterPlants/SnowPea.hpp"
#include "Gameplay/Plants/ShooterPlants/Repeater.hpp"

class World {
private:
    DayMap dayMap;
    Grid grid;

    // Original PopCap viewport
    float GRID_START_X = 40.0f;  
    float GRID_START_Y = 80.0f;  
    static float CELL_WIDTH[], CELL_HEIGHT[];
    float HORIZONTAL_SCALE, VERTICAL_SCALE;
    float GRID_WIDTH, GRID_HEIGHT;
    const float VIRTUAL_WIDTH = 800.0f, VIRTUAL_HEIGHT = 600.0f;

    // float gridX, gridY, cellWidth, cellHeight;
    int selectedPlantId;

    static const char* plantNames[];
    static const int NUM_PLANTS;
    static const int NUM_ROWS = 5, NUM_COLS = 9;

    TextureManager* textureManager = nullptr;
    AnimationManager* animationManager = nullptr;

    std :: unique_ptr<Plant> createPlant(int row, int col, const std::string& plantID);
    bool screenToGrid(float sx, float sy, int& row, int& col) const;
    Rectangle cellScreenRect(int row, int col) const;

    void loadAssets();
public:
    World() = default;
    ~World();
    World(int screenWidth, int screenHeight);

    void update(float dt);
    void draw();
    void onMouseClick(Vector2 position);

    int selectedId() const { return selectedPlantId; }
    void selectPlant(int id) { selectedPlantId = id; }
};

#endif
