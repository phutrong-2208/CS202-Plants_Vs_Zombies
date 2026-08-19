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
    Texture2D* sliderSlot = nullptr;
    Texture2D* sliderKnob = nullptr;
    TextManager* textManager = nullptr;

    bool resumeHovered = false;
    bool mainMenuHovered = false;
    bool musicSliderHovered = false;
    bool soundSliderHovered = false;
    bool draggingMusic = false;
    bool draggingSound = false;

    bool isSettingsMenu = false;

    float getScale() const;
    Vector2 getOffset() const;
    Rectangle toScreenBounds(Rectangle virtualBounds) const;
    void drawButton(Rectangle bounds, const char* text, bool hovered, Color textColor = Color{55, 52, 78, 255}) const;
    void updateSliderFromMouse(bool isMusic, float mouseVirtualX);

public:
    PauseScreen(int screenWidth, int screenHeight, AssetManager* manager, const ScreenData& data = {});

    void update(float dt) override;
    void draw() override;
    void handleInput(const RawInputEvent& inputEvent) override;
};

#endif
