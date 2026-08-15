#ifndef SEED_BANK_HPP
#define SEED_BANK_HPP

#include <Common.hpp>
#include <Core/TexturePackage.hpp>
#include <Core/TextManager.hpp>
#include <Gameplay/Plants/Plant.hpp>

class SeedBank {
private:
    TexturePackage* chooserPackage = nullptr;
    TexturePackage* packetPackage  = nullptr;
    TextManager*    textManager    = nullptr;

    std::map<PlantType, int> sunCosts;
    std::map<PlantType, float> seedRechargeTimes;
    std::map<PlantType, float> cooldownRemaining;
    std::vector<PlantType> slots;
    int selectedSlot = -1;

    Rectangle getSlotRect(int slotIndex) const;
    bool pointInRect(Vector2 position, Rectangle rect) const;
    float getCooldownRatio(PlantType type) const;
    void drawCooldownOverlay(PlantType type, Rectangle bounds) const;

public:
    SeedBank();

    void setTexturePackage(TexturePackage* package);
    void setPacketPackage(TexturePackage* package);
    void setTextManager(TextManager* manager);
    void setSunCosts(const std::map<PlantType, int>& costs);
    void setSeedRechargeTimes(const std::map<PlantType, float>& rechargeTimes);
    void setSlots(const std::vector<PlantType>& selectedPlants);

    void update(float dt);
    void draw() const;
    bool handleMouseClick(Vector2 position);
    void startCooldown(PlantType type);
    bool isCoolingDown(PlantType type) const;

    int selectedPlantId() const;
    void clearSelection();
};

#endif
