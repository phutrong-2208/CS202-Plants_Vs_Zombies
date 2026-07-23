#include "Gameplay/Zombies/Zombie.hpp"

ZombieData::ZombieData(int health, float speed, int damage)
    : baseHealth(health), moveSpeed(speed), attackDamage(damage) {}

int ZombieData::getBaseHealth() const { return baseHealth; }
float ZombieData::getMoveSpeed() const { return moveSpeed; }
int ZombieData::getAttackDamage() const { return attackDamage; }
float ZombieData::getReanimScalar() const { return reanimScalar; }
const std::string& ZombieData::getReanimPackage() const { return reanimPackage; }
const std::string& ZombieData::getReanimAnim() const { return reanimAnim; }
const std::string& ZombieData::getReanimClip() const { return reanimClip; }

void ZombieData::setBaseHealth(int health) { baseHealth = health; }
void ZombieData::setMoveSpeed(float speed) { moveSpeed = speed; }
void ZombieData::setAttackDamage(int damage) { attackDamage = damage; }
void ZombieData::setReanimScalar(float scalar) { reanimScalar = scalar; }
void ZombieData::setReanimPackage(const std::string& package) { reanimPackage = package; }
void ZombieData::setReanimAnim(const std::string& animation) { reanimAnim = animation; }
void ZombieData::setReanimClip(const std::string& clip) { reanimClip = clip; }

void Zombie::zombieSetup() {
    if(!zombieData) {
        health = attackDamage = 0;
        speed = 0.0f;
        return;
    }

    health = zombieData->getBaseHealth();
    speed = zombieData->getMoveSpeed();
    attackDamage = zombieData->getAttackDamage();
}

void Zombie::setZombieData(ZombieData* data) {
    zombieData = data;
    zombieSetup();
}

void Zombie::updateTime(float dt) {
    animation.updateTime(dt);
    if(!attacking) hitbox.x -= speed * dt;
}

void Zombie::setReanimInstance(ReanimInstance anim) { animation = anim; }
void Zombie::draw() { animation.draw(hitbox); }

void Zombie::receiveDamage(int damage) {
    health = std::max(0, health - damage);
}

bool Zombie::isDead() const { return health == 0; }
int Zombie::getHealth() const { return health; }
float Zombie::getSpeed() const { return speed; }
int Zombie::getAttackDamage() const { return attackDamage; }
Rectangle Zombie::getHitbox() const { return hitbox; }

void Zombie::setHitbox(Rectangle newHitbox) { hitbox = newHitbox; }
void Zombie::setAttacking(bool isAttacking) { attacking = isAttacking; }
bool Zombie::isAttacking() const { return attacking; }
