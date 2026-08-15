#include <UI/EndlessHUD.hpp>
#include <Core/TextManager.hpp>
#include <string>

void EndlessHUD :: draw(const EndlessController& ctrl, int highestScore, 
    TextManager* text, float screenW, float screenH) {
    if (!text) return;

    const float x = screenW - 300.0f;
    const float y = 14.0f;

    DrawRectangleRounded({x, y, 210.0f, 72.0f}, 0.25f, 8, Color{30, 30, 45, 200});
    DrawRectangleRoundedLinesEx({x, y, 210.0f, 72.0f}, 0.25f, 8, 2.0f, Color{180, 200, 120, 255});

    text->drawCenteredText("LUCKIEST_GUY",
        ("POINT: " + std::to_string(ctrl.getScore())).c_str(),
        {x, y + 8, 210, 26}, 18, 1.0f, WHITE);
    text->drawCenteredText("LUCKIEST_GUY",
        ("HIGHEST: " + std::to_string(highestScore)).c_str(),
        {x, y + 38, 210, 26}, 16, 1.0f, Color{255, 225, 120, 255});
}
