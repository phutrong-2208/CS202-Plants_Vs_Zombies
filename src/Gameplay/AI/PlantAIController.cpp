#include <Gameplay/AI/PlantAIController.hpp>
#include <Worlds/World.hpp>

namespace {
    constexpr float DECISION_INTERVAL = 0.75f;
    constexpr int LANE_COUNT = 5;
}

void PlantAIController :: setWorld(World* targetWorld) {
    world = targetWorld; 
    rechargeTimes = world ? world -> getAllSeedRecharges() : std :: map<PlantType, float>{};
    sunCosts = world ? world -> getAllSunCosts() : std :: map<PlantType, int>{};
    reset();
}

void PlantAIController::reset() {
    decisionTimer = 0.0f;
    elapsedTime = 0.0f;
    cooldownRemaining.clear();
    retreatLevel.fill(0);
    plantSnapshotInitialized = false;
    for (auto& lane : previousPlants) lane.fill(PLANT_COUNT);
}

void PlantAIController::update(float dt) {
    if (!world || !world->isReady() || dt <= 0.0f) return;

    elapsedTime += dt;
    updatePlantLossMemory();

    for (auto& [type, remaining] : cooldownRemaining) {
        remaining = std::max(0.0f, remaining - dt);
    }

    decisionTimer -= dt;
    if (decisionTimer > 0.0f) return;
    decisionTimer = DECISION_INTERVAL;

    float totalDefenseDeficit = 0.0f;
    for (int lane = 0; lane < LANE_COUNT; ++lane) {
        const float threat = world -> getZombieThreatInLane(lane);
        totalDefenseDeficit += std::max(0.0f, threat - getLaneDefenseScore(lane));
    }

    if (totalDefenseDeficit > 0.0f) {
        // Reinforce only lanes whose current defense is unlikely to win.
        tryEmergencyDefense();
        return;
    }

    // If every active lane is likely to win (or there are no zombies), use
    // the breathing room to improve the Sunflower economy.
    if (tryGrowEconomy()) return;
    trySpendSurplusSun();
}

bool PlantAIController :: tryEmergencyDefense() {
    std :: array<std::pair<float, int>, LANE_COUNT> lanes;
    for (int lane = 0; lane < LANE_COUNT; ++lane) {
        const float threat = world -> getZombieThreatInLane(lane);
        lanes[lane] = {std::max(0.0f, threat - getLaneDefenseScore(lane)), lane};
    }
    std::sort(lanes.begin(), lanes.end(), [](const auto& left, const auto& right) {
        return left.first > right.first;
    });

    for (const auto& [deficit, lane] : lanes) {
        if (deficit <= 0.0f) break;
        const float threat = world -> getZombieThreatInLane(lane);
        const int nearestColumn = world -> getNearestZombieColumnInLane(lane);
        const int zombieCount = world -> getZombieCountInLane(lane);
        const int nearbyZombieCount = nearestColumn >= 0
            ? world -> getZombieCountNearCell(lane, nearestColumn, 1, 1)
            : 0;

        if ((nearbyZombieCount >= 3 || threat >= 16.0f) &&
            tryPlaceNearColumn(lane, std::max(1, nearestColumn), CHERRYBOMB)) {
            return true;
        }

        const int peashooterCount = countPlantTypeInLane(lane, PEASHOOTER);
        if (peashooterCount == 0) {
            if (tryPlaceCloserToHouse(lane, 2, PEASHOOTER)) return true;
            continue;
        }

        if ((zombieCount >= 2 || threat >= 7.0f) && peashooterCount < 2) {
            if (tryPlaceCloserToHouse(lane, 1, PEASHOOTER)) return true;
            continue;
        }

        if (nearestColumn >= 7 && threat >= 3.3f &&
            !hasPlantTypeInLane(lane, POTATOMINE)) {
            const int mineColumn = std::clamp(nearestColumn - 3, 3, 5);
            if (tryPlaceNearColumn(lane, mineColumn, POTATOMINE)) return true;
        }

        if (nearestColumn >= 0 && nearestColumn <= 6) {
            const int barrierColumn = std::clamp(
                nearestColumn - 1 - retreatLevel[lane], 0, 6
            );
            if (tryPlace(lane, barrierColumn, WALLNUT)) return true;
        }

        if (threat >= 4.5f && !hasPlantTypeInLane(lane, SNOWPEA)) {
            if (tryPlaceCloserToHouse(lane, 3, SNOWPEA)) return true;
            continue;
        }
        if (threat >= 10.0f && !hasPlantTypeInLane(lane, REPEATER)) {
            if (tryPlaceCloserToHouse(lane, 4, REPEATER)) return true;
            continue;
        }
    }
    return false;
}

bool PlantAIController::tryGrowEconomy() {
    int targetSunflowers = 3;
    if (elapsedTime >= 30.0f) targetSunflowers = 5;
    if (elapsedTime >= 75.0f) targetSunflowers = 8;
    if (countPlants(SUNFLOWER) >= targetSunflowers) return false;

    std::array<std::pair<float, int>, LANE_COUNT> lanes;
    for (int lane = 0; lane < LANE_COUNT; ++lane) {
        lanes[lane] = {world -> getZombieThreatInLane(lane), lane};
    }
    std::sort(lanes.begin(), lanes.end(), [](const auto& left, const auto& right) {
        return left.first < right.first;
    });

    for (int column : {0, 1}) {
        for (const auto& [threat, lane] : lanes) {
            if (world -> getGrid().getPlant(lane, column)) continue;
            if (tryPlace(lane, column, SUNFLOWER)) return true;
        }
    }
    return false;
}

bool PlantAIController::trySpendSurplusSun() {
    constexpr int SURPLUS_THRESHOLD = 300;
    constexpr int EMERGENCY_RESERVE = 200;

    if (!world || world -> getSunAmount() < SURPLUS_THRESHOLD) return false;

    std::array<int, LANE_COUNT> lanes = {0, 1, 2, 3, 4};
    std::sort(lanes.begin(), lanes.end(), [this](int left, int right) {
        return getLaneDefenseScore(left) < getLaneDefenseScore(right);
    });

    for (int lane : lanes) {
        if (!hasPlantTypeInLane(lane, PEASHOOTER) &&
            tryPlaceCloserToHouseWithReserve(lane, 2, PEASHOOTER, EMERGENCY_RESERVE)) return true;

        if (!hasPlantTypeInLane(lane, SNOWPEA) &&
            tryPlaceCloserToHouseWithReserve(lane, 3, SNOWPEA, EMERGENCY_RESERVE)) return true;

        if (!hasPlantTypeInLane(lane, REPEATER) &&
            tryPlaceCloserToHouseWithReserve(lane, 4, REPEATER, EMERGENCY_RESERVE)) return true;

        if (countPlantTypeInLane(lane, PEASHOOTER) < 2 &&
            tryPlaceCloserToHouseWithReserve(lane, 1, PEASHOOTER, EMERGENCY_RESERVE)) return true;

        if (!hasPlantTypeInLane(lane, WALLNUT) &&
            tryPlaceWithReserve(lane, 6, WALLNUT, EMERGENCY_RESERVE)) return true;
    }
    return false;
}

bool PlantAIController::tryPlace(int lane, int column, PlantType type) {
    if (!world || world->getGrid().getPlant(lane, column) || isCoolingDown(type)) return false;
    if (!world->tryPlacePlantAtCell(lane, column, type)) return false;

    const auto recharge = rechargeTimes.find(type);
    if (recharge != rechargeTimes.end()) {
        cooldownRemaining[type] = std::max(0.0f, recharge -> second);
    }
    return true;
}

bool PlantAIController::tryPlaceWithReserve(int lane, int column, PlantType type, int reserve) {
    const auto cost = sunCosts.find(type);
    if (!world || cost == sunCosts.end()) return false;
    if (world -> getSunAmount() - cost -> second < reserve) return false;
    return tryPlace(lane, column, type);
}

bool PlantAIController::tryPlaceCloserToHouse(int lane, int normalColumn, PlantType type) {
    const int preferredColumn = std::max(0, normalColumn - retreatLevel[lane]);
    for (int column = preferredColumn; column >= 0; --column) {
        if (world -> getGrid().getPlant(lane, column)) continue;
        if (tryPlace(lane, column, type)) return true;
    }
    for (int column = preferredColumn + 1; column <= normalColumn; ++column) {
        if (world -> getGrid().getPlant(lane, column)) continue;
        if (tryPlace(lane, column, type)) return true;
    }
    return false;
}

bool PlantAIController::tryPlaceCloserToHouseWithReserve(int lane, int normalColumn, PlantType type, int reserve) {
    const int preferredColumn = std::max(0, normalColumn - retreatLevel[lane]);
    for (int column = preferredColumn; column >= 0; --column) {
        if (world -> getGrid().getPlant(lane, column)) continue;
        if (tryPlaceWithReserve(lane, column, type, reserve)) return true;
    }
    for (int column = preferredColumn + 1; column <= normalColumn; ++column) {
        if (world -> getGrid().getPlant(lane, column)) continue;
        if (tryPlaceWithReserve(lane, column, type, reserve)) return true;
    }
    return false;
}

bool PlantAIController::tryPlaceNearColumn(int lane, int targetColumn, PlantType type) {
    targetColumn = std::clamp(targetColumn, 0, 8);
    for (int distance = 0; distance < 9; ++distance) {
        const int leftColumn = targetColumn - distance;
        if (leftColumn >= 0 && !world -> getGrid().getPlant(lane, leftColumn) &&
            tryPlace(lane, leftColumn, type)) return true;

        const int rightColumn = targetColumn + distance;
        if (distance > 0 && rightColumn < 9 &&
            !world -> getGrid().getPlant(lane, rightColumn) &&
            tryPlace(lane, rightColumn, type)) return true;
    }
    return false;
}

bool PlantAIController::isCoolingDown(PlantType type) const {
    const auto found = cooldownRemaining.find(type);
    return found != cooldownRemaining.end() && found -> second > 0.0f;
}

int PlantAIController::countPlants(PlantType type) const {
    if (!world) return 0;
    int count = 0;
    for (int lane = 0; lane < LANE_COUNT; ++lane) {
        for (int column = 0; column < 9; ++column) {
            Plant* plant = world -> getGrid().getPlant(lane, column);
            if (plant && plant -> getType() == type) count++;
        }
    }
    return count;
}

bool PlantAIController::hasPlantTypeInLane(int lane, PlantType type) const {
    if (!world || lane < 0 || lane >= LANE_COUNT) return false;
    for (int column = 0; column < 9; ++column) {
        Plant* plant = world -> getGrid().getPlant(lane, column);
        if (plant && plant -> getType() == type) return true;
    }
    return false;
}

int PlantAIController::countPlantTypeInLane(int lane, PlantType type) const {
    if (!world || lane < 0 || lane >= LANE_COUNT) return 0;
    int count = 0;
    for (int column = 0; column < 9; ++column) {
        Plant* plant = world -> getGrid().getPlant(lane, column);
        if (plant && plant -> getType() == type) count++;
    }
    return count;
}

float PlantAIController::getLaneDefenseScore(int lane) const {
    if (!world || lane < 0 || lane >= LANE_COUNT) return 0.0f;

    const int nearestZombieColumn = world -> getNearestZombieColumnInLane(lane);
    float score = 0.0f;
    for (int column = 0; column < 9; ++column) {
        Plant* plant = world -> getGrid().getPlant(lane, column);
        if (!plant || plant -> isDead()) continue;

        // A plant already behind the closest zombie can no longer protect the
        // house from that zombie, so do not count it as active defense.
        if (nearestZombieColumn >= 0 && column > nearestZombieColumn) continue;

        switch (plant -> getType()) {
            case PEASHOOTER:
                score += 3.2f;
                break;
            case SNOWPEA:
                score += 5.0f;
                break;
            case REPEATER:
                score += 6.2f;
                break;
            case WALLNUT:
                score += std::clamp(plant -> getHealth() / 1000.0f, 0.5f, 3.5f);
                break;
            case POTATOMINE:
                score += 2.5f;
                break;
            default:
                break;
        }
    }
    return score;
}

void PlantAIController::updatePlantLossMemory() {
    if (!world) return;

    for (int lane = 0; lane < LANE_COUNT; ++lane) {
        bool lostPlantThisFrame = false;
        for (int column = 0; column < 9; ++column) {
            Plant* plant = world -> getGrid().getPlant(lane, column);
            const PlantType currentType = plant ? plant -> getType() : PLANT_COUNT;
            if (plantSnapshotInitialized &&
                previousPlants[lane][column] != PLANT_COUNT &&
                previousPlants[lane][column] != CHERRYBOMB &&
                previousPlants[lane][column] != POTATOMINE &&
                currentType == PLANT_COUNT) {
                lostPlantThisFrame = true;
            }
            previousPlants[lane][column] = currentType;
        }

        if (lostPlantThisFrame && world -> getZombieThreatInLane(lane) > 0.0f) {
            retreatLevel[lane] = std::min(2, retreatLevel[lane] + 1);
        }
    }
    plantSnapshotInitialized = true;
}
