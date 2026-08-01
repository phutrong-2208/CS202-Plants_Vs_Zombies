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
        PEASHOOTER, SUNFLOWER, CHERRYBOMB, WALLNUT, POTATOMINE,
        SNOWPEA, CHOMPER, REPEATER, PUFFSHROOM, SUNSHROOM,
        FUMESHROOM, GRAVEBUSTER, HYPNOSHROOM, SCAREDYSHROOM,
        ICESHROOM, DOOMSHROOM, LILYPAD, SQUASH, THREEPEATER,
        TANGLEKELP, JALAPENO, CALTROP, TORCHWOOD, TALLNUT,
        SEASHROOM, PLANTERN, CACTUS, BLOVER, SPLITPEA, STARFRUIT,
        PUMPKIN, MAGNETSHROOM, CABBAGEPULT, CORNPULT, COFFEEBEAN,
        GARLIC, UMBRELLALEAF, MARIGOLD, MELONPULT, GATLINGPEA,
        TWINSUNFLOWER, GLOOMSHROOM, CATTAIL, SPIKEROCK, GOLDMAGNET,
        WINTERMELON, COBCANNON, IMITATER
    };
    return plants;
}

PlantData::PlantData(float baseHealth, float baseDamage, float buffDamage, int sunCost, float projectileCooldown) {
    this -> baseHealth = baseHealth;
    this -> baseDamage = baseDamage;
    this -> buffDamage = buffDamage;
    this -> sunCost = sunCost;
    this -> projectileCooldown = projectileCooldown;
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
float PlantData::getProjectileCooldown() const {
    return projectileCooldown;
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

void PlantData::setBaseHealth(float health) { baseHealth = health; }
void PlantData::setBaseDamage(float damage) { baseDamage = damage; }
void PlantData::setBuffDamage(float damage) { buffDamage = damage; }
void PlantData::setSunCost(int cost) { sunCost = cost; }
void PlantData::setProjectileCooldown(float cooldown) { projectileCooldown = cooldown; }

void Plant::plantSetup() {
    cooldownTimer = 0.0f;
    if (plantData == nullptr) {
        health = 0; return;
    }
    health = plantData -> getBaseHealth();
}

void Plant::updateTime(float deltaSeconds) {
    animation.updateTime(deltaSeconds);

    if(cooldownTimer > 0.0f){
        cooldownTimer -= deltaSeconds;
    }
}
void Plant::resetCooldown() {
    cooldownTimer = plantData -> getProjectileCooldown();
}

void Plant::setBounds(Rectangle newBounds) {
    bounds = newBounds;
}
Rectangle Plant::getBounds() const {
    return bounds;
}


Vector2 Plant::getProjectileSpawnPosition() {
    return {0.0f, 0.0f};
}
PlantType Plant::getType() {
    return PLANT_COUNT;
}


void Plant::draw(Rectangle hitbox) {
    animation.draw(hitbox);   
}
void Plant::setReanimInstance(ReanimInstance anim) {
    animation = anim;
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
