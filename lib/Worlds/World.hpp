#ifndef WORLD_HPP
#define WORLD_HPP

#include "Worlds/Grid.hpp"
#include "Worlds/DayMap.hpp"

class World {
private:
    DayMap dayMap;
    Grid grid;
    float gridX, gridY, cellWidth, cellHeight;
    int selectedPlantId;

    static const char* plantNames[];
    static const int NUM_PLANTS;

    std :: unique_ptr<Plant> createPlant(int id, Vector2 pos);

    bool screenToGrid(int sx, int sy, int& row, int& col) const;
    Rectangle cellScreenRect(int row, int col) const;

public:
    World(int screenWidth, int screenHeight);

    void update(float dt);
    void draw() const;
    void onMouseClick(int sx, int sy);

    int selectedId() const { return selectedPlantId; }
    void selectPlant(int id) { selectedPlantId = id; }
};

#endif
