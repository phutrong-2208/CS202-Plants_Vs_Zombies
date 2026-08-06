#ifndef WAVE_MANAGER_HPP
#define WAVE_MANAGER_HPP

#include <Common.hpp>
#include <Gameplay/Levels/WaveConfig.hpp>

class IGameplayMediator;

class WaveManager {
private:
    WaveList waves;

    int   currentWave     = 0;
    int   pendingIndex    = 0;
    float spawnTimer      = 0.0f;

    int   totalZombies    = 0;
    int   spawnedZombies  = 0;

    bool  waitingForClear = false;
    bool  finished        = false;
    float interWaveDelay  = 0.0f;

public:
    WaveManager() = default;

    void loadLevel(int level);
    void update(float dt, IGameplayMediator& mediator);

    float getProgress()    const;
    int   getCurrentWave() const;
    int   getTotalWaves()  const;
    bool  isFinished()     const;
};

#endif
