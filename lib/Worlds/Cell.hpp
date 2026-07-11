#ifndef CELL_HPP
#define CELL_HPP

#include <Common.hpp>
#include "Gameplay/Plants/Plant.hpp"

class Cell {
private:
    std :: unique_ptr<Plant> plant = nullptr;
public:
    Cell();
    ~Cell();

    bool isOccupied() const;

    void updateTime(float deltaSeconds);
    void draw(Rectangle hitbox);
    
    bool placePlant(std::unique_ptr<Plant> newPlant);
    Plant* getPlant() const;
    void removePlant();
};

#endif
