#include <UI/SeedBank.hpp>

namespace {
    constexpr float BANK_X = 10.0f;
    constexpr float BANK_Y = 0.0f;
    constexpr float SLOT_X = BANK_X + 92.0f;
    constexpr float SLOT_Y = BANK_Y + 8.0f;
    constexpr float SLOT_WIDTH = 70.0f;
    constexpr float SLOT_HEIGHT = 92.0f;
    constexpr float SLOT_GAP = 4.0f;

    static const char* plantName(PlantType type) {
        switch (type) {
            case PEASHOOTER: return "PEASHOOTER";
            case SUNFLOWER: return "SUNFLOWER";
            case REPEATER: return "REPEATER";
            case SNOWPEA: return "SNOWPEA";
            case WALLNUT: return "WALLNUT";
            case CACTUS: return "CACTUS";
            default: return "?";
        }
    }
}

SeedBank :: SeedBank() {
    slots = {PEASHOOTER, SUNFLOWER, REPEATER, SNOWPEA, WALLNUT, CACTUS};
}

void SeedBank :: setTexturePackage(TexturePackage* package) {
    chooserPackage = package;
}

void SeedBank :: setPacketPackage(TexturePackage* package) {
    packetPackage = package;
}

void SeedBank :: setSlots(const std::vector<PlantType>& selectedPlants) {
    slots = selectedPlants;
    selectedSlot = -1;
}

void SeedBank :: draw() const {
    Texture2D* seedBank = chooserPackage ? chooserPackage -> GetTexture("SEEDBANK") : nullptr;
    Texture2D* seedPacket = chooserPackage ? chooserPackage -> GetTexture("SEEDPACKET_LARGER") : nullptr;

    if (seedBank) {
        Rectangle src = {0.0f, 0.0f, (float)seedBank -> width, (float)seedBank -> height};
        Rectangle dst = {BANK_X, BANK_Y, 541, 110};
        DrawTexturePro(*seedBank, src, dst, {0, 0}, 0, WHITE);
    } else {
        DrawRectangle(BANK_X, BANK_Y, 490, 98, (Color){82, 56, 30, 230});
    }

    for (int i = 0; i < (int)slots.size(); ++i) {
        Rectangle slotRect = getSlotRect(i);
        Color tint = (i == selectedSlot) ? (Color){210, 255, 170, 255} : WHITE;

        Texture2D* targetPacket = packetPackage ? packetPackage -> GetTexture(plantName(slots[i])) : nullptr;
        if (targetPacket) {
            DrawTexturePro(*targetPacket, {0, 0, (float)targetPacket -> width, (float)targetPacket -> height},
            slotRect, {0, 0}, 0, tint);
        } else if (seedPacket) {
            DrawTexturePro(*seedPacket, {0, 0, (float)seedPacket -> width, (float)seedPacket -> height},
            slotRect, {0, 0}, 0, tint);
        } else {
            DrawRectangleRec(slotRect, (Color){190, 174, 112, 245});
            DrawText(plantName(slots[i]), slotRect.x + 6, slotRect.y + 46, 10, (Color){40, 60, 30, 255});
        }

        if (i == selectedSlot) {
            DrawRectangleLinesEx(slotRect, 3, LIME);
        }
    }
}

bool SeedBank :: handleMouseClick(Vector2 position) {
    for (int i = 0; i < (int)slots.size(); ++i) {
        if (pointInRect(position, getSlotRect(i))) {
            selectedSlot = (selectedSlot == i) ? -1 : i;
            return true;
        }
    }

    return false;
}

int SeedBank :: selectedPlantId() const {
    if (selectedSlot < 0 || selectedSlot >= (int)slots.size()) {
        return -1;
    }

    return (int)slots[selectedSlot];
}

Rectangle SeedBank :: getSlotRect(int slotIndex) const {
    return {
        SLOT_X + slotIndex * (SLOT_WIDTH + SLOT_GAP),
        SLOT_Y,
        SLOT_WIDTH,
        SLOT_HEIGHT
    };
}

bool SeedBank :: pointInRect(Vector2 position, Rectangle rect) const {
    return position.x >= rect.x && position.x <= rect.x + rect.width &&
           position.y >= rect.y && position.y <= rect.y + rect.height;
}
