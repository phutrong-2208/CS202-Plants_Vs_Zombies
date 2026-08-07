#include <UI/SeedBank.hpp>

namespace {
    constexpr float BANK_X = 10.0f;
    constexpr float BANK_Y = 0.0f;
    constexpr float SLOT_X = BANK_X + 92.0f;
    constexpr float SLOT_Y = BANK_Y + 8.0f;
    constexpr float SLOT_WIDTH = 70.0f;
    constexpr float SLOT_HEIGHT = 92.0f;
    constexpr float SLOT_GAP = 4.0f;

}

SeedBank :: SeedBank() {
    slots = {};
}

void SeedBank :: setTexturePackage(TexturePackage* package) {
    chooserPackage = package;
}

void SeedBank :: setPacketPackage(TexturePackage* package) {
    packetPackage = package;
}

void SeedBank :: setTextManager(TextManager* manager) {
    textManager = manager;
}

void SeedBank :: setSunCosts(const std::map<PlantType, int>& costs) {
    sunCosts = costs;
}

void SeedBank :: setSeedRechargeTimes(const std::map<PlantType, float>& rechargeTimes) {
    seedRechargeTimes = rechargeTimes;
}

void SeedBank :: setSlots(const std::vector<PlantType>& selectedPlants) {
    slots = selectedPlants;
    selectedSlot = -1;
}

void SeedBank :: update(float dt) {
    for(auto& [type, remaining] : cooldownRemaining) remaining = std::max(0.0f, remaining - dt);
}

void SeedBank :: startCooldown(PlantType type) {
    auto found = seedRechargeTimes.find(type);
    if(found == seedRechargeTimes.end()) return;
    cooldownRemaining[type] = std::max(0.0f, found -> second);
    if(selectedPlantId() == static_cast<int>(type)) selectedSlot = -1;
}

bool SeedBank :: isCoolingDown(PlantType type) const {
    auto found = cooldownRemaining.find(type);
    return found != cooldownRemaining.end() && found -> second > 0.0f;
}

float SeedBank :: getCooldownRatio(PlantType type) const {
    auto total = seedRechargeTimes.find(type);
    auto remaining = cooldownRemaining.find(type);
    if(total == seedRechargeTimes.end() || remaining == cooldownRemaining.end() || total -> second <= 0.0f) return 0.0f;
    return std::clamp(remaining -> second / total -> second, 0.0f, 1.0f);
}

void SeedBank :: drawCooldownOverlay(PlantType type, Rectangle bounds) const {
    const float ratio = getCooldownRatio(type);
    if(ratio <= 0.0f) return;

    Texture2D* silhouette = chooserPackage ? chooserPackage -> GetTexture("SEEDPACKETSILHOUETTE") : nullptr;
    Rectangle dst = {bounds.x, bounds.y, bounds.width, bounds.height * ratio};
    if(!silhouette) {
        DrawRectangleRec(dst, Color{40, 40, 40, 190});
        return;
    }

    Rectangle src = {0.0f, 0.0f, static_cast<float>(silhouette -> width), static_cast<float>(silhouette -> height) * ratio};
    for(int layer = 0; layer < 3; ++layer) DrawTexturePro(*silhouette, src, dst, {0.0f, 0.0f}, 0.0f, Color{105, 105, 105, 225});
    DrawRectangleRec(dst, Color{0, 0, 0, 90});
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

        Texture2D* targetPacket = packetPackage ? packetPackage -> GetTexture(getPlantTextureKey(slots[i])) : nullptr;
        if (targetPacket) {
            DrawTexturePro(*targetPacket, {0, 0, (float)targetPacket -> width, (float)targetPacket -> height},
            slotRect, {0, 0}, 0, tint);
        } else if (seedPacket) {
            DrawTexturePro(*seedPacket, {0, 0, (float)seedPacket -> width, (float)seedPacket -> height},
            slotRect, {0, 0}, 0, tint);
        } else {
            DrawRectangleRec(slotRect, (Color){190, 174, 112, 245});
            DrawText(getPlantTextureKey(slots[i]), slotRect.x + 6, slotRect.y + 46, 10, (Color){40, 60, 30, 255});
        }

        if (i == selectedSlot) {
            DrawRectangleLinesEx(slotRect, 3, LIME);
        }

        // Sun cost badge drawn directly on the bottom of the packet
        if (textManager) {
            auto costIt = sunCosts.find(slots[i]);
            if (costIt != sunCosts.end()) {
                const std::string costStr = std::to_string(costIt->second);
                const Rectangle costRect = {
                    slotRect.x,
                    slotRect.y + slotRect.height - 20.0f,
                    slotRect.width,
                    20.0f
                };
                textManager->drawCenteredText("Luckiest_Guy", costStr.c_str(), costRect, 14.0f, 0.5f, Color{255, 230, 50, 255});
            }
        }

        drawCooldownOverlay(slots[i], slotRect);
    }
}

bool SeedBank :: handleMouseClick(Vector2 position) {
    for (int i = 0; i < (int)slots.size(); ++i) {
        if (pointInRect(position, getSlotRect(i))) {
            if(isCoolingDown(slots[i])) return true;
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
