#ifndef WORLD_HPP
#define WORLD_HPP

#include <Worlds/Grid.hpp>
#include <Worlds/DayMap.hpp>

#include "Gameplay/Plants/ShooterPlants/Peashooter.hpp"
#include "Gameplay/Plants/SunProducePlants/Sunflower.hpp"
#include "Gameplay/Plants/DefensivePlants/WallNut.hpp"
#include "Gameplay/Plants/ShooterPlants/SnowPea.hpp"

class World {
private:
    DayMap dayMap;
    Grid grid;
    float gridX, gridY, cellWidth, cellHeight;
    int selectedPlantId;

    TextureManager* file = nullptr;

    static const char* plantNames[];
    static const int NUM_PLANTS;
    const int NUM_ROWS = 5, NUM_COLS = 9;

    std :: unique_ptr<Plant> createPlant(int id, Vector2 pos);

    bool screenToGrid(float sx, float sy, int& row, int& col) const;
    Rectangle cellScreenRect(int row, int col) const;

public:
    World() = default;
    ~World();
    World(int screenWidth, int screenHeight);

    void update(float dt);
    void draw() const;
    void onMouseClick(Vector2 position);

    int selectedId() const { return selectedPlantId; }
    void selectPlant(int id) { selectedPlantId = id; }
};

#endif
