#include "Worlds/Grid.hpp"
#include "Gameplay/Plants/Plant.hpp"

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
