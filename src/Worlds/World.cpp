#include <Worlds/World.hpp>

const char* World :: plantNames[] = {
    "Peashooter",
    "Sunflower",
    "Wall-nut",
    "Snow Pea",
};

const int World :: NUM_PLANTS = sizeof(plantNames) / sizeof(plantNames[0]);

// Original PopCap viewport
float World::CELL_WIDTH[] = {
    85, 75, 85, 80, 80, 85, 75, 80, 90
};
float World::CELL_HEIGHT[] = {
    100, 100, 100, 90, 100
};

void World::loadAssets() {
    std :: string dir = std::string(PROJECT_DIR) + "assets/texture/Plants/SunFlower";
    std :: unique_ptr<TexturePackage> SunFlower = std :: make_unique<TexturePackage>();

    SunFlower -> AddTexture("SUNFLOWER_BLINK1",       dir + "/SunFlower_blink1.png");
    SunFlower -> AddTexture("SUNFLOWER_BLINK2",       dir + "/SunFlower_blink2.png");
    SunFlower -> AddTexture("SUNFLOWER_BOTTOMPETALS", dir + "/SunFlower_bottompetals.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD",         dir + "/SunFlower_head.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_SING1",   dir + "/SunFlower_head_sing1.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_SING2",   dir + "/SunFlower_head_sing2.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_SING3",   dir + "/SunFlower_head_sing3.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_SING4",   dir + "/SunFlower_head_sing4.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_SING5",   dir + "/SunFlower_head_sing5.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_WINK",    dir + "/SunFlower_head_wink.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL1",   dir + "/SunFlower_leftpetal1.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL2",   dir + "/SunFlower_leftpetal2.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL3",   dir + "/SunFlower_leftpetal3.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL4",   dir + "/SunFlower_leftpetal4.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL5",   dir + "/SunFlower_leftpetal5.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL6",   dir + "/SunFlower_leftpetal6.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL7",   dir + "/SunFlower_leftpetal7.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL8",   dir + "/SunFlower_leftpetal8.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL1",  dir + "/SunFlower_rightpetal1.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL2",  dir + "/SunFlower_rightpetal2.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL3",  dir + "/SunFlower_rightpetal3.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL4",  dir + "/SunFlower_rightpetal4.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL5",  dir + "/SunFlower_rightpetal5.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL6",  dir + "/SunFlower_rightpetal6.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL7",  dir + "/SunFlower_rightpetal7.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL8",  dir + "/SunFlower_rightpetal8.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL9",  dir + "/SunFlower_rightpetal9.png");
    SunFlower -> AddTexture("SUNFLOWER_DOUBLE_PETALS", dir + "/SunFlower_double_petals.png");
    SunFlower -> AddTexture("SUNFLOWER_TOPPETALS",    dir + "/SunFlower_toppetals.png");

    textureManager -> addPackage("Sunflower", std :: move(SunFlower));

    auto parser = std::make_unique<ReanimParser>();
    parser->loadFromFile("assets/texture/Plants/SunFlower/SunFlower.reanim");
    animationManager -> addAnimationData("SunflowerAnim", std::move(parser));
}
World :: World(int screenWidth, int screenHeight)
    : selectedPlantId(-1)
{
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

    textureManager = new TextureManager();
    animationManager = new AnimationManager();

    loadAssets();
    // float margin = 40.0f;
    // float topBar = 60.0f;
    // CELL_WIDTH = (screenWidth - 2 * margin) / NUM_COLS;
    // CELL_HEIGHT = (screenHeight - topBar - margin) / NUM_ROWS;
    // grid.setCellWidth(CELL_WIDTH);
    // grid.setCellHeight(CELL_HEIGHT);
    // GRID_START_X = margin;
    // GRID_START_Y = topBar;
}
World::~World() {
    delete textureManager;
    delete animationManager;
}

std::unique_ptr<Plant> World :: createPlant(int row, int col, const std::string& plantID) {
    if (row < 0 || row >= NUM_ROWS || col < 0 || col >= NUM_COLS) return nullptr;

    if (plantID == "Sunflower") {
        std::unique_ptr <Plant> plantPtr = std :: make_unique<Sunflower>(cellScreenRect(row, col));

        ReanimInstance plantReanim;
        plantReanim.setTexturePackage(textureManager -> getPackage("Sunflower"));
        plantReanim.setAnimation(animationManager -> getAnimationData("SunflowerAnim"));
        plantPtr -> setReanimInstance(plantReanim);

        return std::move(plantPtr);
    }
    
    return nullptr; 
    // switch (plantID) {
    //     case 0: return std :: make_unique<Peashooter>(cellScreenRect(row, col));
    //     case 1: return std :: make_unique<Sunflower>(cellScreenRect(row, col));
    //     case 2: return std :: make_unique<WallNut>(cellScreenRect(row, col));
    //     case 3: return std :: make_unique<SnowPea>(cellScreenRect(row, col));
    //     default: return nullptr;
    // }
}

bool World :: screenToGrid(float sx, float sy, int& row, int& col) const {
    row = col = -1;
    if (sx < GRID_START_X || sy < GRID_START_Y) return false;

    sx -= GRID_START_X; sy -= GRID_START_Y;
    if (sx > GRID_WIDTH || sy > GRID_HEIGHT) return false;

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

    return true;
}

Rectangle World :: cellScreenRect(int row, int col) const {
    float CELL_X = GRID_START_X, CELL_Y = GRID_START_Y;
    for (int i = 0; i < row; ++i) {
        CELL_Y += CELL_HEIGHT[i];
    }
    for (int i = 0; i < col; ++i) {
        CELL_X += CELL_WIDTH[i];
    }

    return Rectangle {CELL_X, CELL_Y, CELL_WIDTH[col], CELL_HEIGHT[row]};
}

void World :: update(float dt) {
    dayMap.update(dt);
    if (dayMap.isReady() == false) return;
    // if (dayMap.isReady()) {
        // Rectangle area = dayMap.getGridArea();
        
        // GRID_START_X = area.x;
        // GRID_START_Y = area.y;
        // CELL_WIDTH = area.width / NUM_COLS;
        // CELL_HEIGHT = area.height / NUM_ROWS;

        // std::cerr << GRID_START_X << ' ' << GRID_START_Y << ' ' << CELL_WIDTH << ' ' << CELL_HEIGHT << std::endl;

        // grid.setCellWidth(CELL_WIDTH);
        // grid.setCellHeight(CELL_HEIGHT);
    // }

    grid.updateTime(dt);
}

void World :: draw() {
    dayMap.draw();
    if (dayMap.isReady() == false) return;

    grid.draw();
    
    Vector2 mouse = GetMousePosition();
    int hovR, hovC;
    screenToGrid(mouse.x, mouse.y, hovR, hovC);

    for (int r = 0; r < NUM_ROWS; ++r) {
        for (int c = 0; c < NUM_COLS; ++c) {
            Rectangle rect = cellScreenRect(r, c);

            if (r == hovR && c == hovC && selectedPlantId >= 0)
                DrawRectangleLinesEx(rect, 3, LIME);

            // Plant* p = grid.getPlant(r, c);
            // if (p) {
            //     DrawCircle((int)(rect.x + rect.width / 2),
            //                (int)(rect.y + rect.height / 2),
            //                rect.width * 0.22f, (Color){80, 200, 80, 255});
            //     DrawText("P", (int)(rect.x + rect.width / 2 - 7),
            //              (int)(rect.y + rect.height / 2 - 10), 20, (Color){20, 80, 20, 255});
            // }
        }
    }

    float barWidth = NUM_PLANTS * 90.0f;
    float barX = GRID_START_X + (GRID_WIDTH - barWidth) / 2.0f;
    for (int i = 0; i < NUM_PLANTS; ++i) {
        float bx = barX + (float)i * 90.0f;
        float by = GRID_START_Y - 30.0f;
        Color bgCol = (i == selectedPlantId) ? (Color){100, 220, 80, 200} : (Color){180, 200, 150, 200};
        DrawRectangle((int)bx, (int)by, 85, 24, bgCol);
        DrawText(plantNames[i], (int)bx + 4, (int)by + 4, 14,
                 (i == selectedPlantId) ? (Color){20, 50, 20, 255} : (Color){40, 60, 30, 255});
    }
}

void World :: onMouseClick(Vector2 position) {
    float barWidth = NUM_PLANTS * 90.0f;
    float barX = GRID_START_X + (GRID_WIDTH - barWidth) / 2.0f;
    for (int i = 0; i < NUM_PLANTS; ++i) {
        float bx = barX + (float)i * 90.0f;
        float by = GRID_START_Y - 30.0f;
        if (position.x >= bx && position.x <= bx + 85 && position.y >= by && position.y <= by + 22) {
            selectedPlantId = (selectedPlantId == i) ? -1 : i;
            return;
        }
    }

    int r, c;
    if (!screenToGrid(position.x, position.y, r, c) || selectedPlantId < 0) return;

    if (!grid.getPlant(r, c))
        grid.placePlant(r, c, createPlant(r, c, plantNames[selectedPlantId]));
}
