#ifndef GAME_RESULT_SCREEN_HPP
#define GAME_RESULT_SCREEN_HPP

#include <Common.hpp>
#include <Gameplay/Animation/ReanimInstance.hpp>
#include <Screens/Screen.hpp>
#include <Screens/ScreenData.hpp>

enum class Reward {
    PLANTS
};

class GameResultScreen : public Screen {
private:
    static constexpr float VIRTUAL_WIDTH = 800.0f;
    static constexpr float VIRTUAL_HEIGHT = 600.0f;

    int screenWidth = 800;
    int screenHeight = 600;
    ScreenData data;

    TexturePackage* resultPackage = nullptr;
    TexturePackage* seedPacketPackage = nullptr;
    TextManager* textManager = nullptr;

    Texture2D* awardBackground = nullptr;
    Texture2D* buttonLeft = nullptr;
    Texture2D* buttonMiddle = nullptr;
    Texture2D* buttonRight = nullptr;
    Texture2D* buttonDownLeft = nullptr;
    Texture2D* buttonDownMiddle = nullptr;
    Texture2D* buttonDownRight = nullptr;

    ReanimInstance loseAnimation;
    float loseAnimationTime = 0.0f;

    float getScale() const;
    Vector2 getOffset() const;
    Rectangle toScreenBounds(Rectangle virtualBounds) const;
    Rectangle getContinueButtonBounds() const;
    bool isContinueButtonHovered() const;

    void drawResultButton(Rectangle bounds, const char* label, bool hovered) const;
    void drawLoseScreen() const;
    void drawWinScreen() const;
    void drawAwardScreen() const;

public:
    GameResultScreen(
        int screenWidth,
        int screenHeight,
        AssetManager* assetManager,
        ScreenData data
    );

    void update(float dt) override;
    void draw() override;
    void handleInput(const RawInputEvent& inputEvent) override;
};

#endif
