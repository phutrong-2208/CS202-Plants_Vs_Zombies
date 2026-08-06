#ifndef MAIN_MENU_SCREEN
#define MAIN_MENU_SCREEN

#include "Screens/Screen.hpp"
#include "Core/AssetManager.hpp"
#include "Gameplay/Animation/ReanimInstance.hpp"
#include "Gameplay/Levels/Level.hpp"
#include <array>

enum class MainMenuAction{
    NONE, 

    CHANGE_PROFILE,
    ZOMBATAR,
    
    START_ADVENTURE, 
    SURVIVAL, 
    MINI_GAMES,
    PUZZLE,

    OPTION, 
    HELP,
    QUIT
};          

struct MainMenuHoverButton {
    Rectangle bounds = {0.0f, 0.0f, 0.0f, 0.0f};
    Vector2 texturePosition = {0.0f, 0.0f};
    Texture2D* highlightTexture = nullptr;
    
    MainMenuAction action = MainMenuAction :: NONE;
    
    bool enabled = true;
    bool hovered = false;
};

struct MainMenuFlowerLabel {
    Rectangle bounds = {0.0f, 0.0f, 0.0f, 0.0f};
    Vector2 texturePosition = {0.0f, 0.0f};

    Texture2D* normalTexture = nullptr;
    Texture2D* hoverTexture = nullptr;

    MainMenuAction action = MainMenuAction :: NONE;

    bool enabled = true;
    bool hovered = false;
};



class MainMenuScreen : public Screen{
private:
    LevelID id;
    int screenW = 0;
    int screenH = 0;
    bool animationReady = false;
    ReanimInstance skyAnimation;
    ReanimInstance mainAnimation;
    ReanimInstance woodSignAnimation;
    std :: array<ReanimInstance, 7> dynamicAnimations;
    std :: array<MainMenuHoverButton, 6> hoverButtons;
    std :: array<MainMenuFlowerLabel, 3> flowerLabels;

    float getMenuScale() const;
    Vector2 getMenuOffset() const;
    Rectangle toScreenBounds(Rectangle virtualBounds) const;
    void loadHoverButtons(TexturePackage* package);
    void loadFlowerLabels(TexturePackage* package);
    bool loadSceneAnimations(ReanimParser* parser, TexturePackage* package);

public:
    MainMenuScreen(int screenWidth, int screenHeight, AssetManager* manager);

    void update(float dt) override;
    void draw(void) override;
    void executeAction(MainMenuAction target);
    void handleInput(const RawInputEvent& inputEvent) override;
};  

#endif
