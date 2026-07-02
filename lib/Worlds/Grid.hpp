#ifndef GRID_HPP
#define GRID_HPP
#include "Cell.hpp"

// class Plant;   // forward declaration — sufficient for unique_ptr parameter

class Grid {
private:
    static const int NUM_ROWS = 5;
    static const int NUM_COLS = 9;

    Cell garden[NUM_ROWS][NUM_COLS];
public:
    void updateTime(float deltaSeconds);
    void draw();
    
    bool placePlant(int row, int col, std :: unique_ptr<Plant> plant);
    void removePlant(int row, int col);
    Plant* getPlant(int row, int col) const;
};

#endif
