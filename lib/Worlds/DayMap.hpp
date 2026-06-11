#ifndef DAYMAP_HPP
#define DAYMAP_HPP

#include "Worlds/Map.hpp"

class DayMap : public Map {
public:
    enum class Phase { PAN_RIGHT, PAN_LEFT, DONE }; // represent the initial phase after starting the game

    DayMap(); 
    void update(float dt) override; //update the state of map 
    void draw() const override;
    Rectangle getGridArea() const override;

private:
    Phase phase;
    float phaseTimer;
    float phaseDuration;
    Rectangle crop;
    Rectangle lawnSrc;
};

#endif
