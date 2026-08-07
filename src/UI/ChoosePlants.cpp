#include <UI/ChoosePlants.hpp>

namespace {
    constexpr float PANEL_X = 10.0f;
    constexpr float PANEL_Y = 105.0f; //below seedbank
    constexpr float PANEL_WIDTH = 541.0f;
    constexpr float PANEL_HEIGHT = 795.0f; //fully of the screen

    constexpr float AVAILABLE_X = PANEL_X + 12.0f;
    constexpr float AVAILABLE_Y = PANEL_Y + 54.0f;
    constexpr float PACKET_WIDTH = 70.0f;
    constexpr float PACKET_HEIGHT = 92.0f; //The same with SeedBank
    constexpr float PACKET_GAP_X = 4.0f;
    constexpr float PACKET_GAP_Y = 4.0f;
    constexpr int AVAILABLE_COLS = 7;

    constexpr float SELECTED_X = PANEL_X + 38.0f;
    constexpr float SELECTED_Y = PANEL_Y + 47.0f;
    constexpr float SELECTED_WIDTH = 62.0f;
    constexpr float SELECTED_HEIGHT = 84.0f;
    constexpr float SELECTED_GAP = 10.0f;
    constexpr float TITLE_Y_OFFSET = 10.0f;
    constexpr float TITLE_HEIGHT = 36.0f;

    const std::string UI_FONT = "LUCKIEST_GUY";

}

ChoosePlants :: ChoosePlants() {
    availablePlants = getAllPlantTypes();
}

void ChoosePlants :: setChooserPackage(TexturePackage* package) {
    chooserPackage = package;
}

void ChoosePlants :: setPacketPackage(TexturePackage* package) {
    packetPackage = package;
}

void ChoosePlants :: setAvailablePlants(const std::vector<PlantType>& plants) {
    availablePlants = plants;
    reset();
}

void ChoosePlants :: setSeedBank(SeedBank * _sBank) {
    seedBank = _sBank;
    syncSeedBank();
}

void ChoosePlants :: setTextManager(TextManager* manager) {
    textManager = manager;
}

void ChoosePlants :: setSunCosts(const std::map<PlantType, int>& costs) {
    sunCosts = costs;
}

void ChoosePlants :: setMaxSlots(int maxPlants) {
    maxSlots = std::max(1, maxPlants);
    if ((int)selectedPlants.size() > maxSlots) {
        selectedPlants.resize(maxSlots);
    }
    syncSeedBank();
}

void ChoosePlants :: setUnlockedPlants(std :: bitset<PLANT_COUNT> list){
    unlockedPlants = list;
    availablePlants.erase(
        std :: remove_if(
            availablePlants.begin(), availablePlants.end(),
            [&](PlantType plant){
                return !unlockedPlants.test(static_cast<int>(plant));
            }
        ),
        availablePlants.end()
    );
}

void ChoosePlants :: reset() {
    selectedPlants.clear();
    done = false;
    syncSeedBank();
}

void ChoosePlants :: draw() const {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 130});

    Texture2D* background = chooserPackage ? chooserPackage -> GetTexture("SEEDCHOOSER_BACKGROUND") : nullptr;
    if (background) {
        DrawTexturePro(*background, {0, 0, (float)background -> width, (float)background -> height},
        {PANEL_X, PANEL_Y, PANEL_WIDTH, PANEL_HEIGHT}, {0, 0}, 0, WHITE);
    } else {
        DrawRectangleRounded({PANEL_X, PANEL_Y, PANEL_WIDTH, PANEL_HEIGHT}, 0.04f, 8, (Color){92, 70, 43, 245});
        DrawRectangleRoundedLines({PANEL_X, PANEL_Y, PANEL_WIDTH, PANEL_HEIGHT}, 0.04f, 8, (Color){224, 204, 139, 255});
    }

    Rectangle titleRect = {PANEL_X, PANEL_Y + TITLE_Y_OFFSET, PANEL_WIDTH, TITLE_HEIGHT};
    if (textManager) { //drawing Choose Your Plants
        textManager -> drawCenteredText(UI_FONT, "CHOOSE YOUR PLANTS", titleRect, 24.0f, 1.0f, (Color){42, 32, 18, 255});
    } else {
        int titleWidth = MeasureText("CHOOSE YOUR PLANTS", 24);
        DrawText("CHOOSE YOUR PLANTS",
                 (int)(titleRect.x + (titleRect.width - titleWidth) / 2.0f),
                 (int)(titleRect.y + (titleRect.height - 24.0f) / 2.0f),
                 24,
                 (Color){42, 32, 18, 255});
    }
    DrawText(TextFormat("%d/%d", (int)selectedPlants.size(), maxSlots),
             (int)(PANEL_X + PANEL_WIDTH - 105), (int)(PANEL_Y + 22), 20, (Color){42, 32, 18, 255});

    //Drawing Packets

    for (int i = 0; i < (int)availablePlants.size(); ++i) {
        Rectangle rect = getAvailableRect(i);
        PlantType plant = availablePlants[i];
        Texture2D* packet = packetPackage ? packetPackage -> GetTexture(getPlantTextureKey(plant)) : nullptr;
        Color tint = isSelected(plant) ? (Color){180, 180, 180, 255} : WHITE;

        if (packet) {
            DrawTexturePro(*packet, {0, 0, (float)packet -> width, (float)packet -> height},
            rect, {0, 0}, 0, tint);
        } else {
            DrawRectangleRounded(rect, 0.06f, 6, (Color){185, 169, 111, 245});
            DrawText(getPlantTextureKey(plant), (int)rect.x + 6, (int)rect.y + 45, 10, (Color){40, 60, 30, 255});
        }

        if (isSelected(plant)) {
            DrawRectangleRec(rect, (Color){0, 0, 0, 90});
            DrawRectangleLinesEx(rect, 4.0f, LIME);
        }

        // Sun cost overlay at the bottom of the packet
        if (textManager) {
            auto costIt = sunCosts.find(plant);
            if (costIt != sunCosts.end()) {
                const std::string costStr = std::to_string(costIt->second);
                const Rectangle costRect = { rect.x, rect.y + rect.height - 20.0f, rect.width, 20.0f };
                textManager->drawCenteredText(UI_FONT, costStr.c_str(), costRect, 13.0f, 0.5f, Color{255, 230, 50, 255});
            }
        }
    }
    //Drawing the Start button
    Rectangle buttonRect = getStartButtonRect();
    bool canStart = (selectedPlants.size() == maxSlots);
    Color buttonColor = canStart ? (Color){228, 191, 73, 255} : (Color){126, 112, 78, 255};
    Color textColor = canStart ? (Color){42, 30, 14, 255} : (Color){70, 62, 48, 255};
    DrawRectangleRounded(buttonRect, 0.12f, 8, buttonColor);
    DrawRectangleRoundedLines(buttonRect, 0.12f, 8, (Color){72, 49, 20, 255});
    if (textManager) {
        textManager -> drawCenteredText(UI_FONT, "LET'S ROCK", buttonRect, 22.0f, 1.0f, textColor);
    } else {
        DrawText("LET'S ROCK", (int)buttonRect.x + 31, (int)buttonRect.y + 18, 22, textColor);
    }
}

bool ChoosePlants :: handleMouseClick(Vector2 position) {
    if (pointInRect(position, getStartButtonRect())) {
        if (!selectedPlants.empty()) {
            done = true;
        }
        return true;
    }

    for (int i = 0; i < (int)availablePlants.size(); ++i) {
        if (!pointInRect(position, getAvailableRect(i))) {
            continue;
        }

        PlantType plant = availablePlants[i];
        auto it = std :: find(selectedPlants.begin(), selectedPlants.end(), plant);
        if (it != selectedPlants.end()) {
            selectedPlants.erase(it);
        } else if ((int)selectedPlants.size() < maxSlots) {
            selectedPlants.push_back(plant);
        }
        syncSeedBank();
        return true;
    }

    return false;
}

bool ChoosePlants :: isDone() const {
    return done;
}

const std::vector<PlantType>& ChoosePlants :: getSelectedPlants() const {
    return selectedPlants;
}

std::vector<PlantType> ChoosePlants :: choosePlants() const {
    return selectedPlants;
}

Rectangle ChoosePlants :: getAvailableRect(int index) const {
    int row = index / AVAILABLE_COLS;
    int col = index % AVAILABLE_COLS;

    return {
        AVAILABLE_X + col * (PACKET_WIDTH + PACKET_GAP_X),
        AVAILABLE_Y + row * (PACKET_HEIGHT + PACKET_GAP_Y),
        PACKET_WIDTH,
        PACKET_HEIGHT
    };
}

Rectangle ChoosePlants :: getSelectedRect(int index) const {
    return {
        SELECTED_X + index * (SELECTED_WIDTH + SELECTED_GAP),
        SELECTED_Y,
        SELECTED_WIDTH,
        SELECTED_HEIGHT
    };
}

Rectangle ChoosePlants :: getStartButtonRect() const {
    return {PANEL_X + PANEL_WIDTH - 210.0f, PANEL_Y + PANEL_HEIGHT - 80.0f, 165.0f, 58.0f};
}

bool ChoosePlants :: isSelected(PlantType type) const {
    return std::find(selectedPlants.begin(), selectedPlants.end(), type) != selectedPlants.end();
}

bool ChoosePlants :: pointInRect(Vector2 position, Rectangle rect) const {
    return position.x >= rect.x && position.x <= rect.x + rect.width &&
           position.y >= rect.y && position.y <= rect.y + rect.height;
}

void ChoosePlants :: syncSeedBank() {
    if (seedBank) {
        seedBank -> setSlots(selectedPlants);
    }
}
