#include <Worlds/World.hpp>
// const char* World :: plantNames[] = {
//     "PeaShooter",
//     "SunFlower",
//     "Repeater",
//     "SnowPea",
//     "WallNut",
// };

const int World :: NUM_PLANTS = PlantType::PLANT_COUNT;

// Original PopCap viewport
float World::CELL_WIDTH[] = {
    85, 75, 85, 80, 80, 85, 75, 80, 90
};
float World::CELL_HEIGHT[] = {
    100, 100, 100, 90, 100
};

World :: World(int screenWidth, int screenHeight)
: selectedPlantId(-1)
{
    HORIZONTAL_SCALE = screenWidth / VIRTUAL_WIDTH;
    VERTICAL_SCALE = screenHeight / VIRTUAL_HEIGHT;

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

    assetManager = std::make_unique <AssetManager> ();
    plantFactory.setTextureManager(assetManager -> getTextureManager());
    plantFactory.setAnimationManager(assetManager -> getAnimationManager());
}

void World :: update(float dt) {
    dayMap.update(dt);
    if (dayMap.isReady() == false) return;

    grid.updateTime(dt);
}

void World :: draw() {
    dayMap.draw();
    if (dayMap.isReady() == false) return;

    grid.draw();
    
    Vector2 mouse = GetMousePosition();
    int hovR, hovC; std::tie(hovR, hovC) = grid.getCellID(mouse);

    if (hovR != -1 && hovC != -1 && selectedPlantId >= 0) {
        Rectangle rect = grid.getCellRect(hovR, hovC);
        DrawRectangleLinesEx(rect, 3, LIME);
    }

    float barWidth = NUM_PLANTS * 90.0f;
    float barX = GRID_START_X;
    for (int i = 0; i < NUM_PLANTS; ++i) {
        float bx = barX + (float)i * 90.0f;
        float by = GRID_START_Y - 30.0f;
        Color bgCol = (i == selectedPlantId) ? (Color){100, 220, 80, 200} : (Color){180, 200, 150, 200};
        DrawRectangle((int)bx, (int)by, 85, 24, bgCol);

        PlantType pType = (PlantType) i;

        #define PROCESS_VAL(p) case(p): return #p;
        auto Testing = [&] () -> std::string {
            switch(pType) {
                PROCESS_VAL(PEASHOOTER);
                PROCESS_VAL(SUNFLOWER);
                PROCESS_VAL(REPEATER);
                PROCESS_VAL(SNOWPEA);
                PROCESS_VAL(WALLNUT);
            }
            return "";
        };
        #undef PROCESS_VAL(p)

        DrawText(Testing().c_str(), (int)bx + 4, (int)by + 4, 14,
                 (i == selectedPlantId) ? (Color){20, 50, 20, 255} : (Color){40, 60, 30, 255});
    }
}

void World :: onMouseClick(Vector2 position) {
    float barWidth = NUM_PLANTS * 90.0f;
    float barX = GRID_START_X;
    for (int i = 0; i < NUM_PLANTS; ++i) {
        float bx = barX + (float)i * 90.0f;
        float by = GRID_START_Y - 30.0f;
        if (position.x >= bx && position.x <= bx + 85 && position.y >= by && position.y <= by + 22) {
            selectedPlantId = (selectedPlantId == i) ? -1 : i;
            return;
        }
    }

    if (selectedPlantId < 0) return;

    int r, c; std::tie(r, c) = grid.getCellID(position);
    if (!grid.getPlant(r, c))
        grid.placePlant(r, c, plantFactory.createPlant((PlantType) selectedPlantId));
}
