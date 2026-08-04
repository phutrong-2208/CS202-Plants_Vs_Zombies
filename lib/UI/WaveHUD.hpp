#ifndef WAVE_HUD_HPP
#define WAVE_HUD_HPP

#include <Common.hpp>
#include <Core/TexturePackage.hpp>

class WaveHUD {
private:
    TexturePackage* wavePackage = nullptr;  // "ZombiePhase" package

    // Layout constants
    static constexpr float BAR_W     = 160.0f;
    static constexpr float BAR_H     = 22.0f;
    static constexpr float ICON_SIZE = 28.0f;

    Rectangle getBarRect(int screenW, int screenH) const;

public:
    WaveHUD() = default;

    void setTexturePackage(TexturePackage* package);
    void draw(float progress, int currentWave, int totalWaves, bool finished,
              int screenW, int screenH) const;
};

#endif
