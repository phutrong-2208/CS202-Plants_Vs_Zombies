#include <Gameplay/Levels/WaveManager.hpp>
#include <Gameplay/IGameplayMediator.hpp>

void WaveManager :: loadWaves(const WaveList& waveList, const std :: vector<int>& lanes, float firstWaveDelay, float waveDelay) {
    waves = &waveList;
    activeLanes = lanes;
    remainingZombies.clear();

    totalZombies = 0;
    for(const WaveConfig& wave : waveList) {
        for(const ZombieWaveEntry& entry : wave.zombies) {
            totalZombies += std :: max(0, entry.count);
        }
    }

    currentWave = -1;
    remainingZombieCount = 0;
    spawnedZombies = 0;
    spawnTimer = 0.0f;
    nextWaveTimer = std :: max(0.0f, firstWaveDelay);
    betweenWaveDelay = std :: max(0.0f, waveDelay);
    waitingForNextWave = true;
    finished = waveList.empty() || activeLanes.empty();

    if(!waveList.empty() && activeLanes.empty()) {
        TraceLog(LOG_ERROR, "WaveManager: level has no active lanes");
    }
}

void WaveManager :: prepareCurrentWave() {
    remainingZombies.clear();
    remainingZombieCount = 0;

    if(!waves || currentWave < 0 || currentWave >= static_cast<int>(waves -> size())) {
        return;
    }

    const WaveConfig& wave = (*waves)[currentWave];
    for(const ZombieWaveEntry& entry : wave.zombies) {
        if(entry.type == ZOMBIE_COUNT || entry.count <= 0) continue;

        remainingZombies.push_back(entry);
        remainingZombieCount += entry.count;
    }

    spawnTimer = 0.0f;
}

ZombieType WaveManager :: takeRandomZombie() {
    if(remainingZombieCount <= 0) return ZOMBIE_COUNT;

    int ticket = GetRandomValue(1, remainingZombieCount);
    for(ZombieWaveEntry& entry : remainingZombies) {
        if(entry.count <= 0) continue;

        ticket -= entry.count;
        if(ticket <= 0) {
            entry.count--;
            remainingZombieCount--;
            return entry.type;
        }
    }

    return ZOMBIE_COUNT;
}

int WaveManager :: chooseRandomLane() const {
    if(activeLanes.empty()) return -1;

    const int index = GetRandomValue(
        0,
        static_cast<int>(activeLanes.size()) - 1
    );
    return activeLanes[index];
}

float WaveManager :: getRandomSpawnInterval() const {
    if(!waves || currentWave < 0 || currentWave >= static_cast<int>(waves -> size())) {
        return 0.0f;
    }

    const WaveConfig& wave = (*waves)[currentWave];
    const float minimum = std :: max(0.0f, wave.spawnIntervalMin);
    const float maximum = std :: max(minimum, wave.spawnIntervalMax);

    if(minimum == maximum) return minimum;

    constexpr int RANDOM_PRECISION = 1000000;
    const float ratio = static_cast<float>(GetRandomValue(0, RANDOM_PRECISION)) /
                        static_cast<float>(RANDOM_PRECISION);
    return minimum + (maximum - minimum) * ratio;
}

void WaveManager :: update(float dt, IGameplayMediator& mediator) {
    if(finished || !waves || waves -> empty()) return;

    if(waitingForNextWave) {
        nextWaveTimer -= dt;
        if(nextWaveTimer > 0.0f) return;

        currentWave++;
        if(currentWave >= static_cast<int>(waves -> size())) {
            finished = true;
            return;
        }

        prepareCurrentWave();
        waitingForNextWave = false;

        if(remainingZombieCount == 0) {
            waitingForNextWave = true;
            nextWaveTimer = betweenWaveDelay;
            return;
        }
    }

    spawnTimer -= dt;
    while(spawnTimer <= 0.0f && remainingZombieCount > 0) {
        const ZombieType type = takeRandomZombie();
        const int lane = chooseRandomLane();

        if(type == ZOMBIE_COUNT || lane < 0) {
            finished = true;
            return;
        }

        mediator.spawnZombie(type, lane);
        spawnedZombies++;

        if(remainingZombieCount > 0) {
            spawnTimer += getRandomSpawnInterval();
        }
    }

    if(remainingZombieCount == 0 && !waitingForNextWave) {
        waitingForNextWave = true;
        nextWaveTimer = betweenWaveDelay;
    }
}

float WaveManager :: getProgress() const {
    if(totalZombies == 0) return 0.0f;
    return std :: min(1.0f, static_cast<float>(spawnedZombies) / totalZombies);
}

int WaveManager :: getCurrentWave() const {
    return std :: max(0, currentWave);
}

int WaveManager :: getTotalWaves() const {
    return waves ? static_cast<int>(waves -> size()) : 0;
}

bool WaveManager :: isFinished() const {
    return finished;
}

bool WaveManager :: hasSpawnAll(void) const{
    return totalZombies > 0 and spawnedZombies >= totalZombies;
}

bool WaveManager :: isCurrentWaveHuge(void) const {
    if(!waves || currentWave < 0 || currentWave >= static_cast<int>(waves -> size())) return false;
    return (*waves)[currentWave].isHugeWave;
}

bool WaveManager :: isCurrentWaveFinal(void) const {
    if(!waves || waves -> empty() || currentWave < 0) return false;
    return currentWave == static_cast<int>(waves -> size()) - 1;
}