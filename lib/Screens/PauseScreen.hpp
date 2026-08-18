#ifndef PAUSE_SCREEN_HPP
#define PAUSE_SCREEN_HPP

#include <Screens/Screen.hpp>

class PauseScreen : public Screen {
private:
    int screenWidth = 0;
    int screenHeight = 0;

    Texture2D* menuBackground = nullptr;
    Texture2D* buttonNormal = nullptr;
    Texture2D* buttonHover = nullptr;
    TextManager* textManager = nullptr;

    bool resumeHovered = false;
    bool mainMenuHovered = false;
    bool soundToggleHovered = false;

    float getScale() const;
    Vector2 getOffset() const;
    Rectangle toScreenBounds(Rectangle virtualBounds) const;
    void drawButton(Rectangle bounds, const char* text, bool hovered) const;

public:
    PauseScreen(int screenWidth, int screenHeight, AssetManager* manager);

    void update(float dt) override;
    void draw() override;
    void handleInput(const RawInputEvent& inputEvent) override;
};

#endif
