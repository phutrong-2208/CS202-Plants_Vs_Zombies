#ifndef WAVE_MANAGER_HPP
#define WAVE_MANAGER_HPP

#include <Common.hpp>
#include <Gameplay/Levels/WaveConfig.hpp>
#include <Gameplay/IGameplayMediator.hpp>

class WaveManager {
private:
    const WaveList* waves = nullptr;
    std :: vector<int> activeLanes;
    std :: vector<ZombieWaveEntry> remainingZombies;

    int currentWave = -1;
    int remainingZombieCount = 0;
    float spawnTimer = 0.0f;
    float nextWaveTimer = 0.0f;
    float betweenWaveDelay = 0.0f;

    int totalZombies = 0;
    int spawnedZombies = 0;

    bool waitingForNextWave = false;
    bool finished = false;

    void prepareCurrentWave();
    ZombieType takeRandomZombie();
    int chooseRandomLane() const;
    float getRandomSpawnInterval() const;

public:
    WaveManager() = default;

    void loadWaves(const WaveList& waveList, const std :: vector<int>& lanes, float firstWaveDelay, float betweenWaveDelay);
    void update(float dt, IGameplayMediator& mediator);

    float getProgress(void) const;
    int getCurrentWave(void) const;
    int getTotalWaves(void) const;
    bool isFinished(void) const;
    bool hasSpawnAll(void) const;
};

#endif
