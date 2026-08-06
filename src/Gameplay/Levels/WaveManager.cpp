#include <Gameplay/Levels/WaveManager.hpp>
#include <Gameplay/IGameplayMediator.hpp>

void WaveManager::loadLevel(int level) {
    waves.clear();

    // Wave 1 - 3 zombies, spread out over 20s
    waves.push_back({
        {NORMAL_ZOMBIE, 1, 0.0f},
        {NORMAL_ZOMBIE, 3, 10.0f},
        {NORMAL_ZOMBIE, 0, 20.0f},
    });

    // Wave 2 - 4 zombies, over 24s
    waves.push_back({
        {NORMAL_ZOMBIE, 2, 0.0f},
        {NORMAL_ZOMBIE, 4, 8.0f},
        {NORMAL_ZOMBIE, 0, 16.0f},
        {NORMAL_ZOMBIE, 3, 24.0f},
    });

    // Wave 3 - 5 zombies, over 20s
    waves.push_back({
        {NORMAL_ZOMBIE, 0, 0.0f},
        {NORMAL_ZOMBIE, 1, 5.0f},
        {NORMAL_ZOMBIE, 2, 10.0f},
        {NORMAL_ZOMBIE, 3, 15.0f},
        {NORMAL_ZOMBIE, 4, 20.0f},
    });

    // Wave 4 - 6 zombies, over 15s
    waves.push_back({
        {NORMAL_ZOMBIE, 1, 0.0f},
        {NORMAL_ZOMBIE, 0, 3.0f},
        {NORMAL_ZOMBIE, 3, 6.0f},
        {NORMAL_ZOMBIE, 2, 9.0f},
        {NORMAL_ZOMBIE, 4, 12.0f},
        {NORMAL_ZOMBIE, 0, 15.0f},
    });

    // Wave 5 - HUGE WAVE: 10 zombies rapid-fire across all lanes
    waves.push_back({
        {NORMAL_ZOMBIE, 0, 0.0f},
        {NORMAL_ZOMBIE, 1, 0.3f},
        {NORMAL_ZOMBIE, 2, 0.6f},
        {NORMAL_ZOMBIE, 3, 0.9f},
        {NORMAL_ZOMBIE, 4, 1.2f},
        {NORMAL_ZOMBIE, 0, 1.5f},
        {NORMAL_ZOMBIE, 2, 1.8f},
        {NORMAL_ZOMBIE, 1, 2.1f},
        {NORMAL_ZOMBIE, 3, 2.4f},
        {NORMAL_ZOMBIE, 4, 2.7f},
    });

    // Tally total
    totalZombies = 0;
    for (auto& wave : waves) totalZombies += (int)wave.size();

    currentWave     = -1; // -1 means waiting for the very first wave to start
    pendingIndex    = 0;
    spawnedZombies  = 0;
    waitingForClear = true; // Initial delay before the game really starts
    finished        = false;
    spawnTimer      = 0.0f;
    interWaveDelay  = 18.0f; // Give the player 18 seconds to place some sunflowers!
}

void WaveManager::update(float dt, IGameplayMediator& mediator) {
    if (finished || waves.empty()) return;

    // Between waves: count down then start the next one
    if (waitingForClear) {
        interWaveDelay -= dt;
        if (interWaveDelay > 0.0f) return;

        currentWave++;
        if (currentWave >= (int)waves.size()) {
            finished = true;
            return;
        }
        pendingIndex    = 0;
        spawnTimer      = 0.0f;
        waitingForClear = false;
    }

    // Spawn pending zombies in the current wave
    const auto& waveEntries = waves[currentWave];
    spawnTimer += dt;

    while (pendingIndex < (int)waveEntries.size()) {
        const WaveEntry& entry = waveEntries[pendingIndex];
        if (spawnTimer < entry.delay) break;

        mediator.spawnZombie(entry.type, entry.lane);
        spawnedZombies++;
        pendingIndex++;
    }

    // All zombies in this wave spawned — wait 10s then advance
    if (pendingIndex >= (int)waveEntries.size() && !waitingForClear) {
        waitingForClear = true;
        interWaveDelay  = 10.0f;
    }
}

float WaveManager::getProgress() const {
    if (totalZombies == 0) return 0.0f;
    return std::min(1.0f, (float)spawnedZombies / (float)totalZombies);
}

int WaveManager::getCurrentWave()  const { return std::max(0, currentWave); }
int WaveManager::getTotalWaves()   const { return (int)waves.size(); }
bool WaveManager::isFinished()     const { return finished; }
