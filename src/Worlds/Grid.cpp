#include "Worlds/Grid.hpp"
#include "Gameplay/Plants/Plant.hpp"

Grid :: Grid() : cellWidth(80.0f), cellHeight(100.0f){
    recalculateBounds();
}

void Grid :: setCellWidth(float w) {
    cellWidth = w;
    recalculateBounds();
}

void Grid :: setCellHeight(float h) {
    cellHeight = h;
    recalculateBounds();
}

void Grid :: recalculateBounds() {
    for (int r = 0; r < NUM_ROW; ++r) {
        for (int c = 0; c < NUM_COL; ++c) {
            Rectangle bounds;
            bounds.x      = static_cast<float>(c) * cellWidth;
            bounds.y      = static_cast<float>(r) * cellHeight;
            bounds.width  = cellWidth;
            bounds.height = cellHeight;
            garden[r][c].setBounds(bounds);
        }
    }
}

bool Grid :: placePlant(int row, int col, std::unique_ptr<Plant> plant) {
    if (row < 0 || row >= NUM_ROW || col < 0 || col >= NUM_COL) {
        return false;
    }
    return garden[row][col].placePlant(std :: move(plant));
}

void Grid :: removePlant(int row, int col) {
    if (row >= 0 && row < NUM_ROW && col >= 0 && col < NUM_COL) {
        garden[row][col].removePlant();
    }
}

Plant* Grid :: getPlant(int row, int col) const {
    if (row < 0 || row >= NUM_ROW || col < 0 || col >= NUM_COL) {
        return nullptr;
    }
    return garden[row][col].getPlant();
}
