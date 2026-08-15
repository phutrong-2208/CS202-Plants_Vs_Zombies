#ifndef ENDLESS_HUD_HPP
#define ENDLESS_HUD_HPP

#include <Gameplay/Endless/EndlessController.hpp>

class TextManager;

class EndlessHUD {
public:
    void draw(const EndlessController& ctrl, int highestScore, TextManager* text, float screenW, float screenH);
};

#endif
