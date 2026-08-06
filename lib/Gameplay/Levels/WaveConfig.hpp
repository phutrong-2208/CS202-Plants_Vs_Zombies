#ifndef WAVE_CONFIG_HPP
#define WAVE_CONFIG_HPP

#include <Common.hpp>
#include <Gameplay/Zombies/ZombieType.hpp>

struct WaveEntry {
    ZombieType type;
    int lane;       // Row index, counted from top to bottom.
    float delay;    // Seconds from the beginning of the wave.
};

using WaveList = std :: vector<std :: vector<WaveEntry>>;

#endif
