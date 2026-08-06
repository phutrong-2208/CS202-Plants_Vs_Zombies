#ifndef WAVE_CONFIG_HPP
#define WAVE_CONFIG_HPP

#include <Common.hpp>
#include <Gameplay/Zombies/ZombieType.hpp>

struct ZombieWaveEntry {
    ZombieType type = ZOMBIE_COUNT;
    int count = 0;
};

struct WaveConfig {
    std :: vector<ZombieWaveEntry> zombies;

    float spawnIntervalMin = 0.0f;
    float spawnIntervalMax = 0.0f;

    bool isHugeWave = false;
};

using WaveList = std :: vector<WaveConfig>;

#endif
