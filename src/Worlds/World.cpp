#include <Worlds/World.hpp>
namespace PlantBar {
    const float BOX_WIDTH = 100.0f;
    const float BOX_HEIGHT = 30.0f;
    const float BOX_HORIZONTAL_SPACING = 5.0f;
    const float BAR_X = 40.0f;
    const float BAR_Y = 40.0f;

    static const char* NAMES[] = {
        "PEASHOOTER", "SUNFLOWER", "REPEATER", "SNOWPEA", "WALLNUT"
    };
    static_assert(sizeof(NAMES) / sizeof(NAMES[0]) == PlantType::PLANT_COUNT,
                  "PlantBar::NAMES must match PLANT_COUNT");
}

const int World :: NUM_PLANTS = PlantType::PLANT_COUNT;

World :: World(int screenWidth, int screenHeight)
: selectedPlantId(-1)
{
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

    // Plant selection bar
    using namespace PlantBar;

    for (int i = 0; i < NUM_PLANTS; ++i) {
        float edgeX = BAR_X + i * (BOX_WIDTH + BOX_HORIZONTAL_SPACING);
        Color bgCol = (i == selectedPlantId) ? (Color){100, 220, 80, 200} : (Color){180, 200, 150, 200};
        DrawRectangle(edgeX, BAR_Y, BOX_WIDTH, BOX_HEIGHT, bgCol);

        DrawText(NAMES[i], edgeX, BAR_Y, 14,
                 (i == selectedPlantId) ? (Color){20, 50, 20, 255} : (Color){40, 60, 30, 255});
    }
}

void World :: onMouseClick(Vector2 position) {
    using namespace PlantBar;

    for (int i = 0; i < NUM_PLANTS; ++i) {
        float edgeX = BAR_X + i * (BOX_WIDTH + BOX_HORIZONTAL_SPACING);
        if (position.x >= edgeX && position.x <=  edgeX + BOX_WIDTH && position.y >= BAR_Y && position.y <= BAR_Y + BOX_HEIGHT) {
            selectedPlantId = (selectedPlantId == i) ? -1 : i;
            return;
        }
    }

    if (selectedPlantId < 0) return;

    int r, c; std::tie(r, c) = grid.getCellID(position);
    if (r < 0 || c < 0) return;
    if (!grid.getPlant(r, c))
        grid.placePlant(r, c, plantFactory.createPlant((PlantType) selectedPlantId));
}
