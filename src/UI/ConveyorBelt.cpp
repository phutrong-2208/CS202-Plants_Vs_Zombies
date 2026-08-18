#include <UI/ConveyorBelt.hpp>

namespace {
    constexpr float BELT_X = 10.0f;
    constexpr float BELT_Y = 0.0f;
    constexpr float BELT_WIDTH = 837.0f;
    constexpr float BELT_HEIGHT = 110.0f;
    constexpr float PACKET_X = BELT_X + 18.0f;
    constexpr float PACKET_Y = BELT_Y + 9.0f;
    constexpr float PACKET_WIDTH = 70.0f;
    constexpr float PACKET_HEIGHT = 92.0f;
    constexpr float PACKET_GAP = 4.0f;
    constexpr float PACKET_SLIDE_SPEED = 120.0f;
    constexpr float BELT_SCROLL_SPEED = 45.0f;

}

const char* getZombiePacketTextureKey(ZombieType type) {
    static const char* keys[] = {
        "NORMALZOMBIE", "FLAGZOMBIE", "CONEHEADZOMBIE",
        "POLEVAULTINGZOMBIE", "BUCKETHEADZOMBIE", "NEWSPAPERZOMBIE",
        "SCREENDOORZOMBIE", "FOOTBALLZOMBIE", "DANCINGZOMBIE",
        "BACKUPDANCERZOMBIE", "DUCKYTUBEZOMBIE",
        "DUCKYTUBECONEHEADZOMBIE", "DUCKYTUBEBUCKETHEADZOMBIE",
        "SNORKELZOMBIE", "ZOMBONIZOMBIE", "BOBSLEDTEAMZOMBIE",
        "DOLPHINRIDERZOMBIE", "JACKINTHEBOXZOMBIE", "BALLOONZOMBIE",
        "DIGGERZOMBIE", "POGOZOMBIE", "YETIZOMBIE", "BUNGEEZOMBIE",
        "LADDERZOMBIE", "CATAPULTZOMBIE", "GARGANTUARZOMBIE",
        "IMPZOMBIE", "ZOMBOSSZOMBIE", "ZOMBIECHARRED"
    };

    const int index = static_cast<int>(type);
    if (index < 0 || index >= static_cast<int>(ZOMBIE_COUNT)) return "?";
    return keys[index];
}

const std::vector<ZombieType>& getAllZombiePacketTypes() {
    static const std::vector<ZombieType> zombies = {
        NORMAL_ZOMBIE,
        CONEHEAD_ZOMBIE,
        POLE_VAULTING_ZOMBIE,
        BUCKETHEAD_ZOMBIE,
        NEWSPAPER_ZOMBIE,
        FOOTBALL_ZOMBIE,
        JACK_IN_THE_BOX_ZOMBIE,
        YETI_ZOMBIE,
        GARGANTUAR_ZOMBIE
    };
    return zombies;
}

void ConveyorBelt::setTexturePackage(TexturePackage* package) {
    conveyorPackage = package;
    Texture2D* belt = conveyorPackage ? conveyorPackage->GetTexture("CONVEYORBELT") : nullptr;
    if (belt) SetTextureWrap(*belt, TEXTURE_WRAP_REPEAT);
}

void ConveyorBelt::setPacketPackage(TexturePackage* package) {
    packetPackage = package;
}

void ConveyorBelt::setZombiePool(const std::vector<ZombieType>& zombies) {
    zombiePool.clear();
    for (ZombieType type : zombies) {
        if (type < 0 || type >= ZOMBIE_COUNT || type == ZOMBOSS_ZOMBIE ||
            type == ZOMBIE_CHARRED || type == ZOMBONI_ZOMBIE) continue;
        if (std::find(zombiePool.begin(), zombiePool.end(), type) == zombiePool.end()) {
            zombiePool.push_back(type);
        }
    }
}

void ConveyorBelt::setSpawnIntervalRange(float minimumSeconds, float maximumSeconds) {
    spawnIntervalMin = std::max(0.1f, minimumSeconds);
    spawnIntervalMax = std::max(spawnIntervalMin, maximumSeconds);
    scheduleNextSpawn();
}

void ConveyorBelt::start(int initialPacketCount) {
    packets.clear();
    selectedPacket = -1;
    beltOffset = 0.0f;
    progressionTime = 0.0f;
    scheduleNextSpawn();
    const int count = std::clamp(initialPacketCount, 0, MAX_PACKETS);
    for (int i = 0; i < count; ++i) addPacket(false);
}

void ConveyorBelt::addPacket(bool animateFromRight) {
    if (zombiePool.empty() || static_cast<int>(packets.size()) >= MAX_PACKETS) return;

    const int packetIndex = static_cast<int>(packets.size());
    const float entryX = BELT_X + BELT_WIDTH + 2.0f;
    packets.push_back({
        chooseZombieType(),
        animateFromRight ? entryX : getTargetX(packetIndex)
    });
}

void ConveyorBelt::update(float dt) {
    if (dt <= 0.0f) return;

    progressionTime += dt;

    beltOffset = std::fmod(beltOffset + BELT_SCROLL_SPEED * dt, 512.0f);
    for (int i = 0; i < static_cast<int>(packets.size()); ++i) {
        const float targetX = getTargetX(i);
        if (packets[i].x > targetX) {
            packets[i].x = std::max(targetX, packets[i].x - PACKET_SLIDE_SPEED * dt);
        } else if (packets[i].x < targetX) {
            packets[i].x = std::min(targetX, packets[i].x + PACKET_SLIDE_SPEED * dt);
        }
    }

    if (static_cast<int>(packets.size()) >= MAX_PACKETS || zombiePool.empty()) return;
    spawnTimer -= dt;
    if (spawnTimer <= 0.0f) {
        addPacket(true);
        scheduleNextSpawn();
    }
}

void ConveyorBelt::draw() const {
    Texture2D* backdrop = conveyorPackage ? conveyorPackage->GetTexture("CONVEYORBELT_BACKDROP") : nullptr;
    Texture2D* belt = conveyorPackage ? conveyorPackage->GetTexture("CONVEYORBELT") : nullptr;

    if (backdrop) {
        DrawTexturePro(
            *backdrop,
            {0.0f, 0.0f, static_cast<float>(backdrop->width), static_cast<float>(backdrop->height)},
            {BELT_X, BELT_Y, BELT_WIDTH, BELT_HEIGHT},
            {0.0f, 0.0f}, 0.0f, WHITE
        );
    } else {
        DrawRectangleRec({BELT_X, BELT_Y, BELT_WIDTH, BELT_HEIGHT}, Color{40, 40, 40, 235});
    }

    if (belt) {
        const Rectangle beltBounds = {BELT_X + 6.0f, BELT_Y + 8.0f, BELT_WIDTH - 12.0f, BELT_HEIGHT - 16.0f};
        DrawTexturePro(
            *belt,
            {beltOffset, 0.0f, beltBounds.width, static_cast<float>(belt->height)},
            beltBounds,
            {0.0f, 0.0f}, 0.0f, WHITE
        );
    }

    const Rectangle packetClip = {
        BELT_X + 6.0f,
        BELT_Y + 8.0f,
        BELT_WIDTH - 12.0f,
        BELT_HEIGHT - 16.0f
    };
    BeginScissorMode(
        static_cast<int>(packetClip.x),
        static_cast<int>(packetClip.y),
        static_cast<int>(packetClip.width),
        static_cast<int>(packetClip.height)
    );

    for (int i = 0; i < static_cast<int>(packets.size()); ++i) {
        const Rectangle packetRect = getPacketRect(i);
        const Color tint = i == selectedPacket ? Color{210, 255, 170, 255} : WHITE;
        Texture2D* packetTexture = packetPackage
            ? packetPackage->GetTexture(getZombiePacketTextureKey(packets[i].type))
            : nullptr;

        if (packetTexture) {
            DrawTexturePro(
                *packetTexture,
                {0.0f, 0.0f, static_cast<float>(packetTexture->width), static_cast<float>(packetTexture->height)},
                packetRect,
                {0.0f, 0.0f}, 0.0f, tint
            );
        } else {
            DrawRectangleRec(packetRect, Color{190, 174, 112, 245});
            DrawText(getZombiePacketTextureKey(packets[i].type), static_cast<int>(packetRect.x + 5.0f),
                     static_cast<int>(packetRect.y + 44.0f), 9, BLACK);
        }

        if (i == selectedPacket) {
            Texture2D* glow = packetPackage ? packetPackage->GetTexture("SEEDPACKETGLOW") : nullptr;
            if (glow) {
                const Rectangle glowRect = {
                    packetRect.x - 4.0f, packetRect.y - 4.0f,
                    packetRect.width + 8.0f, packetRect.height + 8.0f
                };
                DrawTexturePro(
                    *glow,
                    {0.0f, 0.0f, static_cast<float>(glow->width), static_cast<float>(glow->height)},
                    glowRect,
                    {0.0f, 0.0f}, 0.0f, WHITE
                );
            } else {
                DrawRectangleLinesEx(packetRect, 3.0f, LIME);
            }
        }
    }
    EndScissorMode();
}

bool ConveyorBelt::handleMouseClick(Vector2 position) {
    const Rectangle packetClip = {
        BELT_X + 6.0f, BELT_Y + 8.0f,
        BELT_WIDTH - 12.0f, BELT_HEIGHT - 16.0f
    };
    if (!CheckCollisionPointRec(position, packetClip)) return false;

    for (int i = static_cast<int>(packets.size()) - 1; i >= 0; --i) {
        if (!CheckCollisionPointRec(position, getPacketRect(i))) continue;
        selectedPacket = selectedPacket == i ? -1 : i;
        return true;
    }
    return false;
}

ZombieType ConveyorBelt::selectedZombieType() const {
    if (selectedPacket < 0 || selectedPacket >= static_cast<int>(packets.size())) {
        return ZOMBIE_COUNT;
    }
    return packets[selectedPacket].type;
}

bool ConveyorBelt::consumeSelectedPacket() {
    if (selectedPacket < 0 || selectedPacket >= static_cast<int>(packets.size())) return false;
    packets.erase(packets.begin() + selectedPacket);
    selectedPacket = -1;
    return true;
}

void ConveyorBelt::clearSelection() {
    selectedPacket = -1;
}

int ConveyorBelt::getPacketCount() const {
    return static_cast<int>(packets.size());
}

Rectangle ConveyorBelt::getPacketRect(int packetIndex) const {
    if (packetIndex < 0 || packetIndex >= static_cast<int>(packets.size())) return {};
    return {packets[packetIndex].x, PACKET_Y, PACKET_WIDTH, PACKET_HEIGHT};
}

float ConveyorBelt::getTargetX(int packetIndex) const {
    return PACKET_X + packetIndex * (PACKET_WIDTH + PACKET_GAP);
}

void ConveyorBelt::scheduleNextSpawn() {
    const int minimumMilliseconds = static_cast<int>(spawnIntervalMin * 1000.0f);
    const int maximumMilliseconds = static_cast<int>(spawnIntervalMax * 1000.0f);
    spawnTimer = GetRandomValue(minimumMilliseconds, maximumMilliseconds) / 1000.0f;
}

ZombieType ConveyorBelt::chooseZombieType() const {
    int totalWeight = 0;
    for (ZombieType type : zombiePool) {
        if (progressionTime < getUnlockTime(type)) continue;
        totalWeight += getSpawnWeight(type);
    }
    if (totalWeight <= 0) return NORMAL_ZOMBIE;

    int ticket = GetRandomValue(1, totalWeight);
    for (ZombieType type : zombiePool) {
        if (progressionTime < getUnlockTime(type)) continue;
        ticket -= getSpawnWeight(type);
        if (ticket <= 0) return type;
    }
    return NORMAL_ZOMBIE;
}

float ConveyorBelt::getUnlockTime(ZombieType type) const {
    switch (type) {
        case NORMAL_ZOMBIE: return 0.0f;
        case CONEHEAD_ZOMBIE:
        case BUCKETHEAD_ZOMBIE: return 35.0f;
        case NEWSPAPER_ZOMBIE: return 70.0f;
        case FOOTBALL_ZOMBIE: return 105.0f;
        case YETI_ZOMBIE: return 140.0f;
        case GARGANTUAR_ZOMBIE: return 180.0f;
        default: return 1000000.0f;
    }
}

int ConveyorBelt::getSpawnWeight(ZombieType type) const {
    switch (type) {
        case NORMAL_ZOMBIE:
            return std::max(32, 68 - static_cast<int>(progressionTime / 5.0f));
        case CONEHEAD_ZOMBIE: return 26;
        case BUCKETHEAD_ZOMBIE: return 18;
        case NEWSPAPER_ZOMBIE: return 16;
        case FOOTBALL_ZOMBIE: return 10;
        case YETI_ZOMBIE: return 6;
        case GARGANTUAR_ZOMBIE: return 3;
        default: return 0;
    }
}
