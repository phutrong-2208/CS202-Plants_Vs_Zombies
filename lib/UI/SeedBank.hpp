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
    std::vector<PlantType> slots;
    int selectedSlot = -1;

    Rectangle getSlotRect(int slotIndex) const;
    bool pointInRect(Vector2 position, Rectangle rect) const;

public:
    SeedBank();

    void setTexturePackage(TexturePackage* package);
    void setPacketPackage(TexturePackage* package);
    void setTextManager(TextManager* manager);
    void setSunCosts(const std::map<PlantType, int>& costs);
    void setSlots(const std::vector<PlantType>& selectedPlants);

    void draw() const;
    bool handleMouseClick(Vector2 position);

    int selectedPlantId() const;
};

#endif
