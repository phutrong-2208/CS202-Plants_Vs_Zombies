#ifndef ENDLESS_WAVE_GENERATOR_HPP
#define ENDLESS_WAVE_GENERATOR_HPP

#include <Gameplay/Levels/WaveConfig.hpp>
#include <Gameplay/Zombies/ZombieType.hpp>
#include <vector>

struct ZombieEntry {
    ZombieType type;
    int cost;
    int unlockAtWave;
    float weight;
};

class EndlessWaveGenerator {
public:
    static constexpr int MAX_BUDGET = 300;
    static constexpr int HUGE_WAVE_INTERVAL = 10;

    WaveConfig generate(int waveIndex) const;

private:
    static const std::vector<ZombieEntry> ZOMBIE_POOL;

    int calcBudget(int waveIndex) const;
    std::vector<ZombieEntry> buildEligiblePool(int waveIndex) const;
    std::vector<ZombieWaveEntry> distributeZombies(
        std::vector<ZombieEntry> pool, int budget) const;
};

#endif
