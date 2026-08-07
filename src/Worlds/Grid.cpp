#include "Worlds/Grid.hpp"
#include "Gameplay/Plants/Plant.hpp"

// Original PopCap viewport constants
namespace {
    constexpr float BASE_CELL_WIDTH[]  = {85, 75, 85, 80, 80, 85, 75, 80, 90};
    constexpr float BASE_CELL_HEIGHT[] = {100, 100, 100, 90, 100};
}

Grid::Grid() {
    HORIZONTAL_SCALE = GetScreenWidth() / VIRTUAL_WIDTH;
    VERTICAL_SCALE   = GetScreenHeight() / VIRTUAL_HEIGHT;

    GRID_START_X *= HORIZONTAL_SCALE;
    GRID_START_Y *= VERTICAL_SCALE;

    // Scale cell dimensions
    GRID_WIDTH = GRID_HEIGHT = 0;
    for (int c = 0; c < NUM_COLS; ++c) {
        cellWidth[c] = BASE_CELL_WIDTH[c] * HORIZONTAL_SCALE;
        GRID_WIDTH  += cellWidth[c];
    }
    for (int r = 0; r < NUM_ROWS; ++r) {
        cellHeight[r] = BASE_CELL_HEIGHT[r] * VERTICAL_SCALE;
        GRID_HEIGHT  += cellHeight[r];
    }

    // Pre-compute every cell rectangle
    float yOff = GRID_START_Y;
    for (int r = 0; r < NUM_ROWS; ++r) {
        float xOff = GRID_START_X;
        for (int c = 0; c < NUM_COLS; ++c) {
            cellRects[r][c] = {xOff, yOff, cellWidth[c], cellHeight[r]};
            xOff += cellWidth[c];
        }
        yOff += cellHeight[r];
    }
}

void Grid::setMediator(IGameplayMediator* mediator) {
    gameplayMediator = mediator;
}

std::pair <int, int> Grid::getCellID(Vector2 position) const {
    float sx = position.x, sy = position.y;
    if (sx < GRID_START_X || sy < GRID_START_Y) return std::make_pair(-1, -1);

    sx -= GRID_START_X; sy -= GRID_START_Y;
    if (sx > GRID_WIDTH || sy > GRID_HEIGHT) return std::make_pair(-1, -1);

    int row = -1, col = -1;
    for (int y = 0; y < NUM_COLS; ++y) {
        if (sx > cellWidth[y]) { sx -= cellWidth[y]; continue; }
        col = y; break;
    }

    for (int x = 0; x < NUM_ROWS; ++x) {
        if (sy > cellHeight[x]) { sy -= cellHeight[x]; continue; }
        row = x; break;
    }

    return std::make_pair(row, col);
}

Rectangle Grid::getCellRect(int row, int col) const {
    if (row < 0 || row >= NUM_ROWS || col < 0 || col >= NUM_COLS)
        return Rectangle {0, 0, 0, 0};

    return cellRects[row][col];
}

void Grid::updateTime(float deltaSeconds) {
    for (int row = 0; row < NUM_ROWS; ++row) {
        for (int col = 0; col < NUM_COLS; ++col) {
            garden[row][col].updateTime(deltaSeconds);

            Plant* plant = getPlant(row, col);
            if (plant == nullptr) continue;

            if (plant -> isDead()) {
                removePlant(row, col);
                continue;
            }
        }
    }
}
void Grid::sendPlantActions() {
    if (gameplayMediator == nullptr) return;

    for (int row = 0; row < NUM_ROWS; ++row) {
        for (int col = 0; col < NUM_COLS; ++col) {
            Plant* plant = getPlant(row, col);
            if (plant == nullptr || plant -> isDead()) continue;

            if (plant -> isOnCooldown()) continue;

            if (plant->getType() == SUNFLOWER || plant->getType() == TWINSUNFLOWER || plant->getType() == SUNSHROOM) {
                Vector2 spawnPos = { plant->getBounds().x + plant->getBounds().width * 0.5f, plant->getBounds().y };
                float targetY = plant->getBounds().y + plant->getBounds().height * 0.5f + GetRandomValue(-20, 20);
                spawnPos.x += GetRandomValue(-20, 20);
                
                gameplayMediator->spawnSun(spawnPos, targetY, 25);
                plant->resetCooldown();
                
                if (plant->getType() == TWINSUNFLOWER) {
                    spawnPos.x += GetRandomValue(-30, 30);
                    targetY += GetRandomValue(-30, 30);
                    gameplayMediator->spawnSun(spawnPos, targetY, 25);
                }
                continue;
            }

            if (!gameplayMediator -> hasTarget(
                plant -> getType(), 
                plant -> getProjectileSpawnPosition(), 
                plant -> getBounds()
            )) continue;

            gameplayMediator -> addProjectile(
                plant -> getType(), 
                plant -> getProjectileSpawnPosition(), 
                plant -> getDamage()
            );

            plant -> resetCooldown();
        }
    }
}

void Grid::draw() {
    for (int row = 0; row < NUM_ROWS; ++row) {
        for (int col = 0; col < NUM_COLS; ++col) {
            if (!garden[row][col].isOccupied()) continue;

            Plant* plant = garden[row][col].getPlant();
            if (plant) {
                plant->draw(cellRects[row][col]);
            }
        }
    }
}

bool Grid :: placePlant(int row, int col, std::unique_ptr<Plant> plant) {
    if (row < 0 || row >= NUM_ROWS || col < 0 || col >= NUM_COLS || !plant) {
        return false;
    }

    plant -> setBounds(cellRects[row][col]);
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

Plant* Grid::getPlantInArea(Rectangle area) const {
    for (int row = 0; row < NUM_ROWS; ++row) {
        for (int col = 0; col < NUM_COLS; ++col) {
            Plant* plant = getPlant(row, col);
            if (plant == nullptr || plant -> isDead()) continue;

            if (CheckCollisionRecs(area, plant -> getHitbox())) return plant;
        }
    }

    return nullptr;
}

bool Grid::hasPlantInArea(Rectangle area) const {
    return getPlantInArea(area) != nullptr;
}

bool Grid::damagePlantInArea(Rectangle area, float damage) {
    Plant* plant = getPlantInArea(area);
    if (plant == nullptr) return false;

    plant -> receiveDamage(static_cast<int>(damage));
    return true;
}
