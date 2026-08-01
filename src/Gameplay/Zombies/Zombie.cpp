#include "Gameplay/Zombies/Zombie.hpp"

ZombieData::ZombieData(float health, float speed, int damage)
    : baseHealth(health), moveSpeed(speed), attackDamage(damage) {}

int ZombieData :: getBaseHealth() const { return baseHealth; }
int ZombieData :: getArmorHealth() const { return armorHealth; }
int ZombieData :: getAlternateHealth() const { return alternateHealth; }
float ZombieData :: getMoveSpeed() const { return moveSpeed; }
int ZombieData :: getAttackDamage() const { return attackDamage; }
float ZombieData :: getAttackInterval() const { return attackInterval; }
int ZombieData :: getUnitCount() const { return unitCount; }
float ZombieData :: getReanimScalar() const { return reanimScalar; }
const std :: string& ZombieData :: getReanimPackage() const { return reanimPackage; }
const std :: string& ZombieData :: getReanimAnim() const { return reanimAnim; }
const std :: string& ZombieData :: getReanimClip() const { return reanimClip; }
const std :: vector<std :: string>& ZombieData :: getHiddenTracks() const { return hiddenTracks; }

void ZombieData :: setBaseHealth(float health) { baseHealth = health; }
void ZombieData :: setArmorHealth(float health) { armorHealth = health; }
void ZombieData :: setAlternateHealth(float health) { alternateHealth = health; }
void ZombieData :: setMoveSpeed(float speed) { moveSpeed = speed; }
void ZombieData :: setAttackDamage(int damage) { attackDamage = damage; }
void ZombieData :: setAttackInterval(float interval) { attackInterval = std :: max(0.0f, interval); }
void ZombieData :: setUnitCount(int count) { unitCount = std :: max(1, count); }
void ZombieData :: setReanimScalar(float scalar) { reanimScalar = scalar; }
void ZombieData :: setReanimPackage(const std :: string& package) { reanimPackage = package; }
void ZombieData :: setReanimAnim(const std :: string& animation) { reanimAnim = animation; }
void ZombieData :: setReanimClip(const std :: string& clip) { reanimClip = clip; }
void ZombieData :: setHiddenTracks(std :: vector<std :: string> tracks) { hiddenTracks = std :: move(tracks); }

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
    if(state == ZombieState::DEAD) return;

    animation.updateTime(dt);
    if(state == ZombieState::WALKING) hitbox.x -= speed * dt;
}

void Zombie::setReanimInstance(ReanimInstance anim) { animation = anim; }
void Zombie::draw() {
    animation.draw(hitbox);
    DrawRectangleLinesEx(getHitbox(), 2.0f, RED);
    DrawRectangleLinesEx(getAttackHitbox(), 2.0f, YELLOW);
}

void Zombie::receiveDamage(float damage) {
    if(state == ZombieState::DEAD || damage <= 0.0f) return;

    health = std::max(0.0f, health - damage);
    if(health == 0.0f) setState(ZombieState::DEAD);
}

bool Zombie::isDead() const { return state == ZombieState::DEAD; }
float Zombie::getHealth() const { return health; }
float Zombie::getSpeed() const { return speed; }
int Zombie::getAttackDamage() const { return attackDamage; }
Rectangle Zombie::getHitbox() const { 
    return{
        hitbox.x + 10.0f,
        hitbox.y + 60.0f,
        30.0f,
        60.0f
    };
}
Rectangle Zombie::getAttackHitbox() const {
    const Rectangle bodyHitbox = getHitbox();
    return {
        bodyHitbox.x - bodyHitbox.width * 0.2f,
        bodyHitbox.y,
        bodyHitbox.width * 0.4f,
        bodyHitbox.height
    };
}
ZombieState Zombie :: getState() const { return state; }

void Zombie :: setHitbox(Rectangle newHitbox) { hitbox = newHitbox; }
void Zombie :: setState(ZombieState newState) {
    if(state == newState) return;

    const ZombieState previousState = state;
    state = newState;
    onStateChanged(previousState, newState);
}
void Zombie :: setAttacking(bool isAttacking) {
    if(state == ZombieState :: DYING || state == ZombieState :: DEAD) return;
    setState(isAttacking ? ZombieState :: EATING : ZombieState :: WALKING);
}

void Zombie :: performAttack(IGameplayMediator& mediator){
    mediator.damagePlantInArea(getAttackHitbox(), attackDamage);
}

void Zombie :: updateCombat(float dt, IGameplayMediator& mediator){
    const bool hasPlant = mediator.hasPlantInArea(getAttackHitbox());

    setAttacking(hasPlant);
    
    if(!hasPlant){
        attackTimer = 0.0f;
        return;
    }

    attackTimer += dt;
    if(attackTimer >= attackInterval){
        mediator.damagePlantInArea(getAttackHitbox(), attackDamage);
        attackTimer = 0.0f;
    }
}


bool Zombie :: isAttacking() const { return state == ZombieState :: EATING; }

void Zombie :: onStateChanged(ZombieState previousState, ZombieState newState) {
    switch(newState){
        case ZombieState :: WALKING:
            animation.playClip("walk");
            animation.setLoopToggle(true);
            break;
        case ZombieState :: EATING:
            animation.playClip("eat");
            animation.setLoopToggle(true);
            break;
        case ZombieState :: DYING:
            animation.playClip("die");
            animation.setLoopToggle(false);
            break;
        case ZombieState :: DEAD:
            break;
    }
}

