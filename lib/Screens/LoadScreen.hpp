#ifndef LOAD_SCREEN_HPP
#define LOAD_SCREEN_HPP

#include <Screens/Screen.hpp>
#include <Gameplay/Animation/ReanimInstance.hpp>
#include <array>

class LoadScreen : public Screen {
private:
    enum class Phase {
        LOADING,
        FINISHING,
        READY
    };

    static constexpr std::size_t SPROUT_COUNT = 5;

    int screenWidth = 0;
    int screenHeight = 0;
    Phase phase = Phase::LOADING;

    Texture2D* background = nullptr;
    Texture2D* logo = nullptr;
    Texture2D* dirtBar = nullptr;
    Texture2D* grassBar = nullptr;
    TextManager* textManager = nullptr;

    std::array<ReanimInstance, SPROUT_COUNT> sprouts;
    std::array<bool, SPROUT_COUNT> sproutActive{};
    ReanimInstance zombieHead;
    bool sproutAnimationReady = false;
    bool zombieAnimationReady = false;

    float displayedProgress = 0.0f;
    float blinkTime = 0.0f;

    float getScale() const;
    Vector2 getOffset() const;
    Rectangle toScreenBounds(Rectangle virtualBounds) const;
    void drawLoadingBar() const;
    void drawStatusText() const;

public:
    LoadScreen(int screenWidth, int screenHeight, AssetManager* manager);

    void update(float dt) override;
    void draw() override;
    void handleInput(const RawInputEvent& inputEvent) override;
};

#endif
