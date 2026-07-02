#include "Worlds/Grid.hpp"
#include "Gameplay/Plants/Plant.hpp"

void Grid::updateTime(float deltaSeconds) {
    for (int row = 0; row < NUM_ROWS; ++row) {
        for (int col = 0; col < NUM_COLS; ++col) {
            garden[row][col].updateTime(deltaSeconds);

            Plant* plant = getPlant(row, col);
            if (plant == nullptr || plant -> isDead())  
                removePlant(row, col);
        }
    }
}
void Grid::draw() {
    for (int row = 0; row < NUM_ROWS; ++row) {
        for (int col = 0; col < NUM_COLS; ++col) {
            garden[row][col].draw();
        }
    }
}

bool Grid :: placePlant(int row, int col, std::unique_ptr<Plant> plant) {
    if (row < 0 || row >= NUM_ROWS || col < 0 || col >= NUM_COLS) {
        return false;
    }
    return garden[row][col].placePlant(std :: move(plant));
}

void Grid :: removePlant(int row, int col) {
    if (row >= 0 && row < NUM_ROWS && col >= 0 && col < NUM_COLS) {
        garden[row][col].removePlant();
    }
}

Plant* Grid :: getPlant(int row, int col) const {
    if (row < 0 || row >= NUM_ROWS || col < 0 || col >= NUM_COLS) {
        return nullptr;
    }
    return garden[row][col].getPlant();
}
