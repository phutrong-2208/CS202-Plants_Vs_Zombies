#include "Gameplay/Plants/Plant.hpp"

const char* getPlantTextureKey(PlantType type) {
    static const char* keys[] = {
        "PEASHOOTER", "SUNFLOWER", "CHERRYBOMB", "WALLNUT",
        "POTATOMINE", "SNOWPEA", "CHOMPER", "REPEATER",
        "PUFFSHROOM", "SUNSHROOM", "FUMESHROOM", "GRAVEBUSTER",
        "HYPNOSHROOM", "SCAREDYSHROOM", "ICESHROOM", "DOOMSHROOM",
        "LILYPAD", "SQUASH", "THREEPEATER", "TANGLEKELP",
        "JALAPENO", "CALTROP", "TORCHWOOD", "TALLNUT",
        "SEASHROOM", "PLANTERN", "CACTUS", "BLOVER",
        "SPLITPEA", "STARFRUIT", "PUMPKIN", "MAGNETSHROOM",
        "CABBAGEPULT", "CORNPULT", "COFFEEBEAN", "GARLIC",
        "UMBRELLALEAF", "MARIGOLD", "MELONPULT", "GATLINGPEA",
        "TWINSUNFLOWER", "GLOOMSHROOM", "CATTAIL", "SPIKEROCK",
        "GOLDMAGNET", "WINTERMELON", "COBCANNON", "IMITATER"
    };

    const int index = static_cast<int>(type);
    if(index < 0 || index >= static_cast<int>(PLANT_COUNT)) return "?";
    return keys[index];
}

const std :: vector<PlantType>& getAllPlantTypes() {
    static const std :: vector<PlantType> plants = {
        // SunProducePlants
        SUNFLOWER, TWINSUNFLOWER, SUNSHROOM, MARIGOLD,
        
        // DefensivePlants
        WALLNUT, TALLNUT, PUMPKIN, GARLIC, UMBRELLALEAF, BLOVER,
        COFFEEBEAN, GOLDMAGNET, GRAVEBUSTER, HYPNOSHROOM, IMITATER,
        LILYPAD, MAGNETSHROOM, PLANTERN, TORCHWOOD,
        
        // ExplosivePlants
        CHERRYBOMB, POTATOMINE, DOOMSHROOM, ICESHROOM, SQUASH, JALAPENO, TANGLEKELP,
        
        // ShooterPlants
        PEASHOOTER, SNOWPEA, CHOMPER, REPEATER, PUFFSHROOM, FUMESHROOM, SCAREDYSHROOM,
        THREEPEATER, CALTROP, SPIKEROCK, SEASHROOM, CACTUS, SPLITPEA, STARFRUIT,
        CABBAGEPULT, CORNPULT, MELONPULT, WINTERMELON, GATLINGPEA, GLOOMSHROOM,
        CATTAIL, COBCANNON
    };
    return plants;
}

PlantData::PlantData(float baseHealth, float baseDamage, float buffDamage, int sunCost, float cooldown) {
    this -> baseHealth = baseHealth;
    this -> baseDamage = baseDamage;
    this -> buffDamage = buffDamage;
    this -> sunCost = sunCost;
    this -> cooldown = cooldown;
}
float PlantData::getBaseHealth() const {
    return baseHealth;
}
float PlantData::getDamage(bool buffed) const {
    return buffed ? buffDamage : baseDamage;
}
int PlantData::getSunCost() const {
    return sunCost;
}
float PlantData::getCooldown() const {
    return cooldown;
}
float PlantData::getSeedRecharge() const {
    return seedRecharge;
}

float PlantData::getReanimScalar() const {
    return reanimScalar;
}
const std::string& PlantData::getReanimPackage() const {
    return reanimPackage;
}
const std::string& PlantData::getReanimAnim() const {
    return reanimAnim;
}
const std::string& PlantData::getReanimClip() const {
    return reanimClip;
}

const std::vector<std::pair<std::string, std::vector<std::string>>>& PlantData::getClipLayers() const {
    return clipLayers;
}

void PlantData::setReanimScalar(float scalar) {
    reanimScalar = scalar;
}
void PlantData::setReanimPackage(const std::string& package) {
    reanimPackage = package;
}
void PlantData::setReanimAnim(const std::string& anim) {
    reanimAnim = anim;
}
void PlantData::setReanimClip(const std::string& clip) {
    reanimClip = clip;
}

void PlantData::addClipLayer(const std::string& clip) {
    clipLayers.push_back({clip, {}});
}

void PlantData::addClipLayerShowTrack(const std::string& track) {
    if (!clipLayers.empty()) {
        clipLayers.back().second.push_back(track);
    }
}

void PlantData::addHiddenTrack(const std::string& track) {
    hiddenTracks.push_back(track);
}

const std::vector<std::string>& PlantData::getHiddenTracks() const {
    return hiddenTracks;
}

void PlantData::setBaseHealth(float health) { baseHealth = health; }
void PlantData::setBaseDamage(float damage) { baseDamage = damage; }
void PlantData::setBuffDamage(float damage) { buffDamage = damage; }
void PlantData::setSunCost(int cost) { sunCost = cost; }
void PlantData::setCooldown(float cd) { cooldown = cd; }
void PlantData::setSeedRecharge(float recharge) { seedRecharge = recharge; }

void Plant::plantSetup() {
    if (plantData == nullptr) {
        cooldownTimer = 0.0f;
        health = 0; return;
    }
    cooldownTimer = plantData->getCooldown();
    health = plantData->getBaseHealth();
}

void Plant::performAction(IGameplayMediator* mediator) {
    if (plantData && plantData->getCooldown() > 0.0f && getProjectileSpawnPosition().x != 0.0f) {
        if (!mediator->hasTarget(getType(), getProjectileSpawnPosition(), getBounds())) return;
        mediator->addProjectile(getType(), getProjectileSpawnPosition(), getDamage());
        resetCooldown();
    }
}

void Plant::triggerAnimation(const std::string& clipName) {
    animation.playClip(clipName);
    animation.setLoopToggle(false);
}

void Plant::updateTime(float deltaSeconds) {
    animation.updateTime(deltaSeconds);
    
    if (!animation.isLooping() && animation.isFinished()) {
        animation.resetToDefault();
    }

    if(cooldownTimer > 0.0f){
        cooldownTimer -= deltaSeconds;
    }
}
void Plant::resetCooldown() {
    cooldownTimer = plantData -> getCooldown();
}

void Plant::setBounds(Rectangle newBounds) {
    bounds = newBounds;
}
Rectangle Plant::getBounds() const {
    return bounds;
}
Rectangle Plant::getHitbox() const {
    return {
        bounds.x + bounds.width * 0.25f,
        bounds.y + bounds.height * 0.25f,
        bounds.width * 0.5f,
        bounds.height * 0.7f
    };
}


Vector2 Plant::getProjectileSpawnPosition() {
    return {0.0f, 0.0f};
}
PlantType Plant::getType() {
    return PLANT_COUNT;
}


void Plant::draw(Rectangle hitbox) {
    animation.draw(hitbox);
    DrawRectangleLinesEx(getHitbox(), 2.0f, GREEN);
}
void Plant::setReanimInstance(ReanimInstance anim) {
    animation = std::move(anim);
}


void Plant::setPlantData(PlantData* pData) {
    plantData = pData;

    plantSetup();
}

void Plant :: receiveDamage(int damage){
    health -= damage;
    if(health < 0) health = 0;
}

bool Plant :: isDead() const {
    return health == 0;
}
bool Plant::isOnCooldown() const {
    return cooldownTimer > 0.0f;
}
float Plant :: getHealth() const {
    return health;
}
int Plant :: getCost() const {
    if (plantData) return plantData -> getSunCost();
    return 0;
}
float Plant::getDamage() const {
    if (plantData) return plantData -> getDamage(buffed);
    return 0.0f;
}
