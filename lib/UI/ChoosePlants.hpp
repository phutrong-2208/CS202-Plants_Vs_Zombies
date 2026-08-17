#ifndef CHOOSE_PLANTS_HPP
#define CHOOSE_PLANTS_HPP

#include <Common.hpp>
#include <Core/TexturePackage.hpp>
#include <Core/TextManager.hpp>
#include <Gameplay/Plants/Plant.hpp>
#include <UI/SeedBank.hpp>

class ChoosePlants {
private:
    TexturePackage* chooserPackage = nullptr;
    TexturePackage* packetPackage = nullptr;


    SeedBank* seedBank = nullptr;
    TextManager* textManager = nullptr;



    std::vector<PlantType> availablePlants;
    std::vector<PlantType> selectedPlants;
    std::map<PlantType, int> sunCosts;
    int maxSlots = 10;
    bool done = false;

    std :: bitset<PLANT_COUNT> unlockedPlants;

    Rectangle getAvailableRect(int index) const;
    Rectangle getSelectedRect(int index) const;
    Rectangle getStartButtonRect() const;
    bool isSelected(PlantType type) const;
    bool pointInRect(Vector2 position, Rectangle rect) const;
    void syncSeedBank();

public:
    ChoosePlants();

    void setChooserPackage(TexturePackage* package);
    void setPacketPackage(TexturePackage* package);
    void setAvailablePlants(const std::vector<PlantType>& plants);
    void setMaxSlots(int maxPlants);
    void setSeedBank(SeedBank* seedBank);
    void setTextManager(TextManager* manager);
    void setSunCosts(const std::map<PlantType, int>& costs);
    void setUnlockedPlants(std :: bitset<PLANT_COUNT> list);
    void reset();

    void draw() const;
    bool handleMouseClick(Vector2 position);

    bool isDone() const;
    const std::vector<PlantType>& getSelectedPlants() const;
    std::vector<PlantType> choosePlants() const;
};

#endif
