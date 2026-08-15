#include <Gameplay/Endless/EndlessWaveGenerator.hpp>
#include <cmath>
#include <algorithm>
#include <unordered_map>

// Heuristic 
// -----------------------------------------------------------------------
const std::vector<ZombieEntry> EndlessWaveGenerator :: ZOMBIE_POOL = {
    // type                       cost  unlock  weight
    {NORMAL_ZOMBIE,               1,    1,    3.5f},
    {CONEHEAD_ZOMBIE,             2,    3,    2.8f},
    {POLE_VAULTING_ZOMBIE,        3,    4,    1.8f},
    {BUCKETHEAD_ZOMBIE,           4,    6,    2.2f},
    {NEWSPAPER_ZOMBIE,            3,    6,    2.0f},
    {SCREEN_DOOR_ZOMBIE,          4,    8,    1.6f},
    {FOOTBALL_ZOMBIE,             6,   10,    1.2f},
    {DANCING_ZOMBIE,              6,   10,    1.0f},
    {BACKUP_DANCER_ZOMBIE,        1,   10,    2.5f},
    {BALLOON_ZOMBIE,              3,   12,    1.4f},
    {POGO_ZOMBIE,                 4,   12,    1.3f},
    {JACK_IN_THE_BOX_ZOMBIE,      5,   14,    1.1f},
    {ZOMBONI_ZOMBIE,              7,   15,    0.9f},
    {BUNGEE_ZOMBIE,               5,   17,    0.8f},
    {LADDER_ZOMBIE,               5,   17,    0.8f},
    {CATAPULT_ZOMBIE,             6,   18,    0.7f},
    {GARGANTUAR_ZOMBIE,          12,   20,    0.5f},
    {IMP_ZOMBIE,                  1,   20,    1.5f},
};

// The heuristic budget calculation for each wave
// Budget(W) = 10 + 5 * W + floor(W ^ 1.35)
// Huge Wave (W % HUGE_WAVE_INTERVAL == 0): X2 

int EndlessWaveGenerator :: calcBudget(int waveIndex) const {
    const bool isHuge = (waveIndex % HUGE_WAVE_INTERVAL == 0);

    int budget = 3 * waveIndex + static_cast<int>(std :: floor(std :: pow(waveIndex, 1.35)));

    if (isHuge) budget = static_cast<int>(budget * 2.0f);

    return std :: min(budget, 300);
}

// Zombie filtering has been unlocked.
std :: vector<ZombieEntry> EndlessWaveGenerator :: buildEligiblePool(int waveIndex) const {
    std :: vector<ZombieEntry> pool(ZOMBIE_POOL.size());
    for (const ZombieEntry& entry : ZOMBIE_POOL) {
        if (entry.unlockAtWave <= waveIndex) {
            pool.push_back(entry);
        }
    }
    return pool;
}

//Using budget to buy zombie
std :: vector<ZombieWaveEntry> EndlessWaveGenerator :: distributeZombies(std :: vector<ZombieEntry> pool, int budget) const {
    std :: unordered_map<int, int> counts; //counts[ZOMBIE] is saving the number of that zombie type
    
    while (budget > 0) {
        float totalWeight = 0.0f;
        for (const ZombieEntry& e : pool) {
            if (e.cost <= budget) totalWeight += e.weight;
        }

        if (totalWeight <= 0.0f) break;

        // Weighted random pick
        float r = static_cast<float>(GetRandomValue(0, 1000000))
                / 1000000.0f * totalWeight;

        ZombieEntry* chosen = nullptr;
        for (ZombieEntry& e : pool) {
            if (e.cost > budget) continue;
            r -= e.weight;
            if (r <= 0.0f) {
                chosen = &e;
                break;
            }
        }
        // Fallback in case floating-point rounding causes the selection to be missed
        if (!chosen) {
            for (ZombieEntry& e : pool) {
                if (e.cost <= budget) { chosen = &e; break; }
            }
        }

        if (!chosen) break;

        counts[static_cast<int>(chosen->type)]++;
        budget -= chosen -> cost;

        // Weight decay: decrease 8% to increase zombie diversity
        chosen -> weight *= 0.92f;
    }

    // Chuyển map counts → vector ZombieWaveEntry
    std :: vector<ZombieWaveEntry> entries;
    entries.reserve(counts.size());
    for (const auto& [typeInt, count] : counts) {
        ZombieWaveEntry entry;
        entry.type  = static_cast<ZombieType>(typeInt);
        entry.count = count;
        entries.push_back(entry);
    }
    return entries;
}

WaveConfig EndlessWaveGenerator :: generate(int waveIndex) const {
    WaveConfig config;

    const bool isHuge = (waveIndex % HUGE_WAVE_INTERVAL == 0);
    config.isHugeWave = isHuge;

    const int budget = calcBudget(waveIndex);

    std::vector<ZombieEntry> pool = buildEligiblePool(waveIndex);
    config.zombies = distributeZombies(pool, budget);

    if (isHuge) { // Supply Flag Zombie without extra cost
        ZombieWaveEntry flag;
        flag.type  = FLAG_ZOMBIE;
        flag.count = 1;
        config.zombies.insert(config.zombies.begin(), flag);
    }

    // Spawn interval
    config.spawnIntervalMin = std :: max(0.5f, 2.8f - waveIndex * 0.04f);
    config.spawnIntervalMax = std :: max(1.0f, 5.0f - waveIndex * 0.06f);

    return config;
}
