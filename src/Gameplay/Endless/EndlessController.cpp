#include <Gameplay/Endless/EndlessController.hpp>
#include <algorithm>

namespace {
    const std :: vector<int> ALL_LANES = {0, 1, 2, 3, 4};
}

void EndlessController::launch() {
    currentWave = 0;
    score = 0;
    multiplier = 1.0f;
    flagsCleared = 0;
    phase = Phase :: BETWEEN_WAVES;
    phaseTimer = FIRST_WAVE_DELAY;
}

void EndlessController :: startWave() {
    if (!waveManager) return;

    currentWave++;
    currentGeneratedWave.clear();
    currentGeneratedWave.push_back(generator.generate(currentWave));

    const float firstDelay = 0.5f;
    waveManager -> loadWaves(currentGeneratedWave, ALL_LANES, firstDelay, BETWEEN_WAVE_DELAY);
    phase = Phase :: WAVE_RUNNING;
}

void EndlessController :: update(float dt, bool waveFinished, bool zombiesEmpty, bool zombieReachedHouse) {
    if (phase == Phase :: GAME_OVER) return;

    if (zombieReachedHouse) {
        phase = Phase :: GAME_OVER;
        return;
    }

    switch (phase) {
        case Phase :: BETWEEN_WAVES:
        case Phase :: FLAG_INTERMISSION: {
            phaseTimer -= dt;
            if (phaseTimer <= 0.0f) startWave();
            break;
        }
        case Phase :: WAVE_RUNNING: {
            if (waveFinished && zombiesEmpty) {
                advanceWave();
            }
            break;
        }
        default:
            break;
    }
}

void EndlessController :: advanceWave() {
    score += currentWave * 50;

    if (currentWave % EndlessWaveGenerator :: HUGE_WAVE_INTERVAL == 0) {
        flagsCleared++;
        multiplier = std :: min(1.0f + 0.1f * flagsCleared, 5.0f);
        score += flagsCleared * 500;
    }

    phase = Phase :: BETWEEN_WAVES;
    phaseTimer = BETWEEN_WAVE_DELAY;
}
