#ifndef GAMEPLAY_SCREEN_HPP
#define GAMEPLAY_SCREEN_HPP

#include <Core/AssetManager.hpp>
#include <Screens/Screen.hpp>
#include <Worlds/World.hpp>
#include <UI/ChoosePlants.hpp>
#include <UI/SeedBank.hpp>

class GameplayScreen : public Screen {
private:
    std::unique_ptr<World> world = nullptr;
    ChoosePlants choosePlants;
    SeedBank seedBank;

public:
    GameplayScreen(int screenWidth, int screenHeight, AssetManager* manager);

    void update(float dt) override;
    void draw() override;
    void handleInput(const RawInputEvent& inputEvent) override;
};

#endif
