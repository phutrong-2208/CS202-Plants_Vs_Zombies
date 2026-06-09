#ifndef GRID_HPP
#define GRID_HPP
#include <memory>
#include "Cell.hpp"

class Plant;   // forward declaration — sufficient for unique_ptr parameter

class Grid {
private:
    static const int NUM_ROW = 5;
    static const int NUM_COL = 9;

    float cellWidth;
    float cellHeight;
    Cell garden[NUM_ROW][NUM_COL];

    void recalculateBounds();

public:
    Grid();

    void setCellWidth(float w);
    void setCellHeight(float h);

    bool placePlant(int row, int col, std :: unique_ptr<Plant> plant);
    void removePlant(int row, int col);
    Plant* getPlant(int row, int col) const;
};

#endif
