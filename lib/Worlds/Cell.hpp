#ifndef CELL_HPP
#define CELL_HPP

#include "raylib.h"
#include "Gameplay/Plants/Plant.hpp"
#include <memory>

class Cell {
private:
    std :: unique_ptr<Plant> plant;
    Rectangle bounds;

public:
    Cell();

    bool isOccupied() const;
    Rectangle getBounds() const;
    void setBounds(Rectangle newBounds);

    bool placePlant(std::unique_ptr<Plant> newPlant);
    Plant* getPlant() const;
    void removePlant();
};

#endif
