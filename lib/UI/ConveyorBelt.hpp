#ifndef CONVEYOR_BELT_HPP
#define CONVEYOR_BELT_HPP

#include <Common.hpp>
#include <Core/TexturePackage.hpp>
#include <Gameplay/Zombies/ZombieType.hpp>

const char* getZombiePacketTextureKey(ZombieType type);
const std::vector<ZombieType>& getAllZombiePacketTypes();

class ConveyorBelt {
private:
    static constexpr int MAX_PACKETS = 10;

    struct Packet {
        ZombieType type = ZOMBIE_COUNT;
        float x = 0.0f;
    };

    TexturePackage* conveyorPackage = nullptr;
    TexturePackage* packetPackage = nullptr;
    std::vector<ZombieType> zombiePool;
    std::vector<Packet> packets;
    float spawnIntervalMin = 7.0f;
    float spawnIntervalMax = 15.0f;
    float spawnTimer = 0.0f;
    float beltOffset = 0.0f;
    float progressionTime = 0.0f;
    int selectedPacket = -1;

    Rectangle getPacketRect(int packetIndex) const;
    float getTargetX(int packetIndex) const;
    void addPacket(bool animateFromRight);
    ZombieType chooseZombieType() const;
    float getUnlockTime(ZombieType type) const;
    int getSpawnWeight(ZombieType type) const;
    void scheduleNextSpawn();

public:
    void setTexturePackage(TexturePackage* package);
    void setPacketPackage(TexturePackage* package);
    void setZombiePool(const std::vector<ZombieType>& zombies);
    void setSpawnIntervalRange(float minimumSeconds, float maximumSeconds);
    void start(int initialPacketCount = 0);

    void update(float dt);
    void draw() const;
    bool handleMouseClick(Vector2 position);

    ZombieType selectedZombieType() const;
    bool consumeSelectedPacket();
    void clearSelection();
    int getPacketCount() const;
};

#endif
