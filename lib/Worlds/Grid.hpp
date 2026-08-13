#ifndef GRID_HPP
#define GRID_HPP

#include <Gameplay/IGameplayMediator.hpp>
#include "Cell.hpp"

class Grid {
private:
    IGameplayMediator* gameplayMediator = nullptr;
    static const int NUM_ROWS = 5, NUM_COLS = 9;

    // Original PopCap viewport
    float GRID_START_X = 40.0f;  
    float GRID_START_Y = 80.0f;  
    float cellWidth[NUM_COLS];
    float cellHeight[NUM_ROWS];
    Rectangle cellRects[NUM_ROWS][NUM_COLS];
    float HORIZONTAL_SCALE, VERTICAL_SCALE;
    float GRID_WIDTH, GRID_HEIGHT;
    const float VIRTUAL_WIDTH = 800.0f, VIRTUAL_HEIGHT = 600.0f;

    Cell garden[NUM_ROWS][NUM_COLS];
public:
    Grid();
    void setMediator(IGameplayMediator* mediator);

    std::pair <int, int> getCellID(Vector2 position) const;
    Rectangle getCellRect(int row, int col) const;

    void updateTime(float deltaSeconds);
    void sendPlantActions();
    void draw();
    
    bool placePlant(int row, int col, std :: unique_ptr<Plant> plant);
    void removePlant(int row, int col);
    Plant* getPlant(int row, int col) const;
    Plant* getPlantInArea(Rectangle area) const;
    bool hasPlantInArea(Rectangle area) const;
    bool damagePlantInArea(Rectangle area, float damage, Zombie* attacker = nullptr);
};

#endif
