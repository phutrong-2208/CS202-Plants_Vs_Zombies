#ifndef WORLD_HPP
#define WORLD_HPP

#include <Core/AssetManager.hpp>

#include <Worlds/Grid.hpp>
#include <Worlds/Map.hpp>
#include <Worlds/DayMap.hpp>
#include <Worlds/SeedBank.hpp>


class World {
private:
    std::unique_ptr <Map> map = nullptr;
    SeedBank seedBank;
    Grid grid;
    PlantFactory plantFactory;
    std::unique_ptr <AssetManager> assetManager = nullptr;
    // static const int NUM_ROWS = 5, NUM_COLS = 9;
public:
    World() = default;
    ~World() = default;
    World(int screenWidth, int screenHeight);

    void update(float dt);
    void draw();
    void onMouseClick(Vector2 position);

    int selectedId() const { return seedBank.selectedPlantId(); }
};

#endif
