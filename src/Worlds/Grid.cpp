#include "Worlds/Grid.hpp"
#include "Gameplay/Plants/Plant.hpp"

// Original PopCap viewport
float Grid::CELL_WIDTH[] = {
    85, 75, 85, 80, 80, 85, 75, 80, 90
};
float Grid::CELL_HEIGHT[] = {
    100, 100, 100, 90, 100
};

Grid::Grid() {
    HORIZONTAL_SCALE = GetScreenWidth() / VIRTUAL_WIDTH;
    VERTICAL_SCALE = GetScreenHeight() / VIRTUAL_HEIGHT;

    GRID_START_X *= HORIZONTAL_SCALE;
    GRID_START_Y *= VERTICAL_SCALE;
    GRID_WIDTH = GRID_HEIGHT = 0;
    for (float& num : CELL_WIDTH) {
        num *= HORIZONTAL_SCALE;
        GRID_WIDTH += num;
    }
    for (float& num : CELL_HEIGHT) {
        num *= VERTICAL_SCALE;
        GRID_HEIGHT += num;
    }
}

std::pair <int, int> Grid::getCellID(Vector2 position) const {
    float sx = position.x, sy = position.y;
    if (sx < GRID_START_X || sy < GRID_START_Y) return std::make_pair(-1, -1);

    sx -= GRID_START_X; sy -= GRID_START_Y;
    if (sx > GRID_WIDTH || sy > GRID_HEIGHT) return std::make_pair(-1, -1);

    int row = -1, col = -1;
    for (int y = 0; y < NUM_COLS; ++y) {
        if (sx > CELL_WIDTH[y]) {
            sx -= CELL_WIDTH[y]; continue;
        }
        col = y; break;
    }

    for (int x = 0; x < NUM_ROWS; ++x) {
        if (sy > CELL_HEIGHT[x]) {
            sy -= CELL_HEIGHT[x]; continue;
        }
        row = x; break;
    }

    return std::make_pair(row, col);
}
Rectangle Grid::getCellRect(int row, int col) const {
    if (std::min(row, col) < 0 || std::max(row - NUM_ROWS, col - NUM_COLS) >= 0) return Rectangle {0, 0, 0, 0};

    float CELL_X = GRID_START_X, CELL_Y = GRID_START_Y;
    for (int i = 0; i < row; ++i) {
        CELL_Y += CELL_HEIGHT[i];
    }
    for (int i = 0; i < col; ++i) {
        CELL_X += CELL_WIDTH[i];
    }

    return Rectangle {CELL_X, CELL_Y, CELL_WIDTH[col], CELL_HEIGHT[row]};
}

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
            garden[row][col].draw(getCellRect(row, col));
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
