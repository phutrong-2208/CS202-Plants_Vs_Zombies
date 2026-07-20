#ifndef SEED_BANK_HPP
#define SEED_BANK_HPP

#include <Common.hpp>
#include <Core/TexturePackage.hpp>
#include <Gameplay/Plants/Plant.hpp>
// This class is for containing plants
class SeedBank {
private:
    TexturePackage* chooserPackage = nullptr;
    TexturePackage* packetPackage = nullptr;

    std::vector<PlantType> slots;
    int selectedSlot = -1;

    Rectangle getSlotRect(int slotIndex) const; //get the Rectangle of the current slot
    bool pointInRect(Vector2 position, Rectangle rect) const;

public:
    SeedBank();

    void setTexturePackage(TexturePackage* package);
    void setPacketPackage(TexturePackage* package);
    void setSlots(const std::vector<PlantType>& selectedPlants);

    void draw() const;
    bool handleMouseClick(Vector2 position);

    int selectedPlantId() const;
};

#endif
