#ifndef ENDLESS_CONTROLLER_HPP
#define ENDLESS_CONTROLLER_HPP

#include <Gameplay/Endless/EndlessWaveGenerator.hpp>
#include <Gameplay/Levels/WaveManager.hpp>
#include <vector>

class EndlessController {
public:
    enum class Phase {
        WAVE_RUNNING,
        BETWEEN_WAVES,
        FLAG_INTERMISSION,
        GAME_OVER
    };

    static constexpr float BETWEEN_WAVE_DELAY = 7.0f;
    static constexpr float FIRST_WAVE_DELAY = 20.0f;
    static constexpr int ACTIVE_ZOMBIE_CAP = 60;

    EndlessController() = default;

    void attachToWaveManager(WaveManager* wm) {waveManager = wm;}
    void launch(void);

    void update(float dt, bool waveFinished, bool zombiesEmpty, bool zombieReachedHouse);

    Phase getPhase(void) const {return phase;}
    int getCurrentWave(void) const {return currentWave;}
    int getFlagsCleared(void) const {return flagsCleared;}
    int getScore(void) const {return score;}
    float getMultiplier(void) const {return multiplier;}

private:
    void startWave(void);
    void advanceWave(void);

    EndlessWaveGenerator generator;
    WaveManager* waveManager = nullptr;

    // Phải lưu là member field — KHÔNG được là biến cục bộ trong startWave()!
    // WaveManager chỉ lưu con trỏ (&waveList), nên WaveList phải sống
    // đủ lâu (ít nhất đến khi wave kế tiếp gọi loadWaves).
    WaveList currentGeneratedWave;

    Phase phase = Phase :: BETWEEN_WAVES;
    int currentWave = 0;
    int flagsCleared = 0;
    float phaseTimer = 0.0f;

    int score = 0;
    float multiplier = 1.0f;
};

#endif
