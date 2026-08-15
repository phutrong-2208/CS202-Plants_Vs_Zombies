#ifndef CELL_HPP
#define CELL_HPP

#include <Common.hpp>
#include "Gameplay/Plants/Plant.hpp"

class Cell {
private:
    std::unique_ptr<Plant> plant = nullptr;     // Normal plant
    std::unique_ptr<Plant> pumpkin = nullptr;   // Pumpkin
public:
    Cell();
    ~Cell();

    bool isOccupied() const;
    bool hasNormalPlant() const;

    void updateTime(float deltaSeconds);
    void draw(Rectangle hitbox);
    
    bool placePlant(std::unique_ptr<Plant> newPlant);
    Plant* getPlant() const;
    Plant* getPumpkin() const;
    
    // Check and remove dead plants
    void checkDeadPlants();
    void removePlant(); // Removes all
};

#endif
