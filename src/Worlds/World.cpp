#include "Worlds/World.hpp"
#include "Gameplay/Plants/ShooterPlants/Peashooter.hpp"
#include "Gameplay/Plants/SunProducePlants/Sunflower.hpp"
#include "Gameplay/Plants/DefensivePlants/WallNut.hpp"
#include "Gameplay/Plants/ShooterPlants/SnowPea.hpp"

const char* World :: plantNames[] = {
    "Peashooter",
    "Sunflower",
    "Wall-nut",
    "Snow Pea",
};

const int World :: NUM_PLANTS = sizeof(plantNames) / sizeof(plantNames[0]);

World :: World(int screenWidth, int screenHeight)
    : selectedPlantId(-1)
{
    float margin = 40.0f;
    float topBar = 60.0f;
    cellWidth = (screenWidth - 2 * margin) / 9.0f;
    cellHeight = (screenHeight - topBar - margin) / 5.0f;
    grid.setCellWidth(cellWidth);
    grid.setCellHeight(cellHeight);
    gridX = margin;
    gridY = topBar;
}

std::unique_ptr<Plant> World :: createPlant(int id, Vector2 pos) {
    switch (id) {
        case 0: return std :: make_unique<Peashooter>(pos);
        case 1: return std :: make_unique<Sunflower>(pos);
        case 2: return std :: make_unique<WallNut>(pos);
        case 3: return std :: make_unique<SnowPea>(pos);
        default: return nullptr;
    }
}

bool World :: screenToGrid(int sx, int sy, int& row, int& col) const {
    float cx = (float)(sx - (int)gridX);
    float cy = (float)(sy - (int)gridY);
    if (cx < 0 || cy < 0) return false;
    col = (int)(cx / cellWidth);
    row = (int)(cy / cellHeight);
    return (row >= 0 && row < 5 && col >= 0 && col < 9);
}

Rectangle World :: cellScreenRect(int row, int col) const {
    return {
        gridX + (float)col * cellWidth,
        gridY + (float)row * cellHeight,
        cellWidth, cellHeight
    };
}

void World :: update(float dt) {
    for (int r = 0; r < 5; ++r)
        for (int c = 0; c < 9; ++c)
            if (Plant* p = grid.getPlant(r, c))
                if (p -> isDead())
                    grid.removePlant(r, c);
}

void World :: draw() const {
    Vector2 mouse = GetMousePosition();
    int hovR = -1, hovC = -1;
    screenToGrid((int)mouse.x, (int)mouse.y, hovR, hovC);

    Rectangle gridRect = {gridX, gridY, 9 * cellWidth, 5 * cellHeight};
    DrawRectangleRec(gridRect, (Color){30, 60, 30, 255});
    DrawRectangleLinesEx(gridRect, 3, (Color){50, 100, 50, 255});

    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            Rectangle rect = cellScreenRect(r, c);
            bool light = (r + c) % 2 == 0;
            Color col = light ? (Color){100, 140, 80, 255} : (Color){80, 120, 60, 255};

            if (r == hovR && c == hovC && selectedPlantId >= 0)
                col = (Color){60, 200, 40, 255};
            else if (r == hovR && c == hovC)
                col = (Color){140, 180, 100, 255};

            DrawRectangleRec(rect, col);
            DrawRectangleLinesEx(rect, 1, (Color){50, 90, 40, 255});

            if (r == hovR && c == hovC && selectedPlantId >= 0) {
                DrawRectangleLinesEx(rect, 3, LIME);
                DrawText("+", (int)(rect.x + rect.width / 2 - 8),
                         (int)(rect.y + rect.height / 2 - 12), 28, LIME);
            }

            Plant* p = grid.getPlant(r, c);
            if (p) {
                DrawCircle((int)(rect.x + rect.width / 2),
                           (int)(rect.y + rect.height / 2),
                           cellWidth * 0.22f, (Color){80, 200, 80, 255});
                DrawText("P", (int)(rect.x + rect.width / 2 - 7),
                         (int)(rect.y + rect.height / 2 - 10), 20, (Color){20, 80, 20, 255});
            }
        }
    }

    float barWidth = NUM_PLANTS * 90.0f;
    float barX = gridX + (9 * cellWidth - barWidth) / 2.0f;
    for (int i = 0; i < NUM_PLANTS; ++i) {
        float bx = barX + (float)i * 90.0f;
        float by = gridY - 30.0f;
        Color bg = (i == selectedPlantId) ? (Color){100, 220, 80, 255} : (Color){180, 200, 150, 255};
        DrawRectangle((int)bx, (int)by, 85, 24, bg);
        DrawText(plantNames[i], (int)bx + 4, (int)by + 4, 14, (i == selectedPlantId) ? (Color){20, 50, 20, 255} : (Color){40, 60, 30, 255});
    }
}

void World :: onMouseClick(int sx, int sy) {
    float barWidth = NUM_PLANTS * 90.0f;
    float barX = gridX + (9 * cellWidth - barWidth) / 2.0f;
    for (int i = 0; i < NUM_PLANTS; ++i) {
        float bx = barX + (float)i * 90.0f;
        float by = gridY - 30.0f;
        if (sx >= bx && sx <= bx + 85 && sy >= by && sy <= by + 22) {
            selectedPlantId = (selectedPlantId == i) ? -1 : i;
            return;
        }
    }

    int r, c;
    if (!screenToGrid(sx, sy, r, c) || selectedPlantId < 0) return;

    if (!grid.getPlant(r, c))
        grid.placePlant(r, c, createPlant(selectedPlantId, Vector2{(float)sx, (float)sy}));
}
