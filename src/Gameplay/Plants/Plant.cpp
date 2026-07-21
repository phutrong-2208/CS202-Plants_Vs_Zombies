#include "Gameplay/Plants/Plant.hpp"

PlantData::PlantData(float baseHealth, float baseDamage, float buffDamage, int sunCost, float projectileCooldown, float projectileRange) {
    this -> baseHealth = baseHealth;
    this -> baseDamage = baseDamage;
    this -> buffDamage = buffDamage;
    this -> sunCost = sunCost;
    this -> projectileCooldown = projectileCooldown;
    this -> projectileRange = projectileRange;
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
float PlantData::getProjectileRange() const {
    return projectileRange;
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

void Plant::plantSetup() {
    if (plantData == nullptr) {
        health = cooldownTimer = 0; return;
    }
    health = plantData -> getBaseHealth();
    cooldownTimer = plantData -> getProjectileCooldown();
}

void Plant::updateTime(float deltaSeconds) {
    animation.updateTime(deltaSeconds);
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

bool Plant :: isDead(void) const {
    if(health == 0) return 1;
    return 0;
}

int Plant :: getHealth(void) const {
    return health;
}

int Plant :: getCost(void) const {
    if (plantData) return plantData -> getSunCost();
    return 0;
}
