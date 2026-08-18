#include "Gameplay/Zombies/Zombie.hpp"
#include "Gameplay/Zombies/ZombieDeathHandler.hpp"
#include "Gameplay/Particle/ZombiePart.hpp"
#include "Gameplay/IGameplayMediator.hpp"

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
        health = maxHealth = attackDamage = 0;
        speed = 0.0f;
        armorHealth = maxArmorHealth = alternateHealth = 0.0f;
        return;
    }

    health = maxHealth = zombieData->getBaseHealth();
    armorHealth = maxArmorHealth = zombieData->getArmorHealth();
    alternateHealth = zombieData->getAlternateHealth();
    speed = zombieData->getMoveSpeed();
    attackDamage = zombieData->getAttackDamage();
}

void Zombie::setZombieType(ZombieType type) {
    zombieType = type;
}

void Zombie::setDeathHandler(ZombieDeathHandler* handler) {
    deathHandler = handler;
}

void Zombie::setZombieData(ZombieData* data) {
    zombieData = data;
    zombieSetup();
}

void Zombie::triggerCharred(ReanimInstance charredAnim) {
    isCharred = true;
    health = 0.0f;
    deathTimer = 0.0f;
    setState(ZombieState::DYING);
    animation = std::move(charredAnim);
    animation.playClip("anim_crumble");
    animation.setLoopToggle(false);
}

void Zombie::updateTime(float dt) {
    if(state == ZombieState::DEAD) return;

    if (freezeTimer > 0.0f) {
        freezeTimer -= dt;
    } else if (chillTimer > 0.0f) {
        chillTimer -= dt;
    }

    float effectiveSpeed = speed;
    float timeMultiplier = 1.0f;
    if (freezeTimer > 0.0f) {
        effectiveSpeed = 0.0f;
        timeMultiplier = 0.0f;
    } else if (chillTimer > 0.0f) {
        effectiveSpeed = speed * 0.5f;
        timeMultiplier = 0.5f;
    }

    if (flashTimer > 0.0f) {
        flashTimer -= dt;
    }

    animation.updateTime(dt * timeMultiplier);
    if(state == ZombieState::WALKING) hitbox.x -= effectiveSpeed * dt * (isHypnotized ? -1.0f : 1.0f);
    
    const float ZOMBIE_DEATH_COUNTDOWN = 3.0f;
    if (state == ZombieState::DYING) {
        deathTimer += dt;

        if (isCharred) {
            if (animation.isFinished() || deathTimer >= ZOMBIE_DEATH_COUNTDOWN) {
                setState(ZombieState::DEAD);
            }
        } else if (deathTimer >= ZOMBIE_DEATH_COUNTDOWN) {
            setState(ZombieState::DEAD);
        }
    }
}

void Zombie::setReanimInstance(ReanimInstance anim) { animation = anim; }
void Zombie::draw() {
    if (isFullyDead()) return;

    Color tint = WHITE;
    if (flashTimer > 0.0f) {
        tint = Color{200, 200, 200, 255}; // Light grey / white flash effect
    } else if (freezeTimer > 0.0f) {
        tint = Color{80, 180, 255, 255}; // Deep frozen ice tint
    } else if (chillTimer > 0.0f) {
        tint = Color{120, 160, 255, 255}; // Chilled blue tint
    } else if (isHypnotized) {
        tint = Color{200, 100, 255, 255}; // Purple tint
    }
    
    animation.draw(hitbox, tint);

    if (freezeTimer > 0.0f) {
        Rectangle zHitbox = getHitbox();
        DrawRectangleRec(zHitbox, Color{100, 200, 255, 70});
        DrawRectangleLinesEx(zHitbox, 2.0f, Color{160, 230, 255, 200});
    }

    DrawRectangleLinesEx(getHitbox(), 2.0f, RED);
    DrawRectangleLinesEx(getAttackHitbox(), 2.0f, YELLOW);
}

void Zombie::onArmorBroken() {
    animation.hideTrack("anim_cone");
    animation.hideTrack("anim_bucket");
    animation.hideTrack("anim_football");
    animation.hideTrack("anim_screendoor");
    animation.hideTrack("anim_paper");
    animation.hideTrack("Zombie_cone");
    animation.hideTrack("Zombie_bucket");
    animation.hideTrack("Zombie_footballhelmet");
    animation.hideTrack("Zombie_screendoor");
    animation.hideTrack("Zombie_paper");
}

void Zombie::onCustomCombat(float dt, IGameplayMediator& mediator) {
    // Default base zombie has no custom combat skills
}

void Zombie::receiveDamage(float damage, IGameplayMediator* mediator) {
    if(state == ZombieState :: DYING || state == ZombieState :: DEAD) return;
    
    float prevDamage = damage;
    if(armorHealth > 0.0f) {
        float appliedArmor = std::min(armorHealth, damage);
        armorHealth -= appliedArmor;
        damage -= appliedArmor;
        if (armorHealth <= 0.0f) {
            onArmorBroken();
        }
    }

    if(prevDamage > 0 && damage == 0) {
        flashTimer = 0.15f;
        return;
    }

    if(alternateHealth > 0.0f) {
        float appliedAlt = std::min(alternateHealth, damage);
        alternateHealth -= appliedAlt;
        damage -= appliedAlt;
    }

    if(damage <= 0.0f) return;

    health -= damage;
    if (health <= 0.0f) {
        health = 0.0f;
        setState(ZombieState :: DYING);
        if (zombieType != ZOMBIE_CHARRED) {
            Color deathTint = WHITE;
            if (freezeTimer > 0.0f) deathTint = Color{80, 180, 255, 255};
            else if (chillTimer > 0.0f) deathTint = Color{120, 160, 255, 255};
            else if (isHypnotized) deathTint = Color{200, 100, 255, 255};

            if (deathHandler) {
                deathHandler->spawnDeathParticles(zombieType, hitbox, 1.0f, deathTint);
            }
            animation.hideTrack("anim_head");
            animation.hideTrack("anim_hair");
            animation.hideTrack("anim_jaw");
            animation.hideTrack("anim_cone");
            animation.hideTrack("anim_bucket");
            animation.hideTrack("anim_football");
            animation.hideTrack("anim_screendoor");
            animation.hideTrack("anim_paper");
            animation.hideTrack("Zombie_head");
            animation.hideTrack("Zombie_jaw");
            animation.hideTrack("Zombie_cone");
            animation.hideTrack("Zombie_bucket");
            animation.hideTrack("Zombie_footballhelmet");
            animation.hideTrack("Zombie_screendoor");
            animation.hideTrack("Zombie_paper");
        }
    } else {
        flashTimer = 0.15f; // 150ms flash on hit
    }
}

bool Zombie::isDead() const { return state == ZombieState::DYING || state == ZombieState::DEAD; }
bool Zombie::isFullyDead() const { return state == ZombieState::DEAD; }
bool Zombie::isDying() const { return state == ZombieState::DYING; }

void Zombie::setSwallowed(bool isSwallowed) { swallowed = isSwallowed; }
bool Zombie::isSwallowed() const { return swallowed; }
void Zombie::setHypnotized(bool hypnotized) { isHypnotized = hypnotized; }
float Zombie::getHealth() const { return health; }
float Zombie::getMaxHealth() const { return maxHealth; }
float Zombie::getSpeed() const { return speed; }
void Zombie::setSpeed(float newSpeed) { speed = newSpeed; }
float Zombie::getArmorHealth() const { return armorHealth; }
void Zombie::setArmorHealth(float armor) { armorHealth = armor; }
int Zombie::getAttackDamage() const { return attackDamage; }
Rectangle Zombie::getHitbox() const { 
    if (zombieType == BACKUP_DANCER_ZOMBIE) {
        return {
            hitbox.x + 10.0f,
            hitbox.y + 15.0f,
            30.0f,
            60.0f
        };
    }
    return {
        hitbox.x + 10.0f,
        hitbox.y + 60.0f,
        30.0f,
        60.0f
    };
}
Rectangle Zombie::getAttackHitbox() const {
    const Rectangle bodyHitbox = getHitbox();
    if (isHypnotized) {
        return {
            bodyHitbox.x + bodyHitbox.width * 0.8f,
            bodyHitbox.y,
            bodyHitbox.width * 0.4f,
            bodyHitbox.height
        };
    }
    return {
        bodyHitbox.x - bodyHitbox.width * 0.2f,
        bodyHitbox.y,
        bodyHitbox.width * 0.4f,
        bodyHitbox.height
    };
}
ZombieState Zombie :: getState() const { return state; }
ZombieType Zombie::getType() const { return zombieType; }

void Zombie :: setHitbox(Rectangle newHitbox) { hitbox = newHitbox; }
void Zombie :: setState(ZombieState newState) {
    if(state == newState) return;

    state = newState;
    onStateChanged(newState);
}
void Zombie :: setAttacking(bool isAttacking) {
    if(state == ZombieState :: DYING || state == ZombieState :: DEAD) return;
    setState(isAttacking ? ZombieState :: EATING : ZombieState :: WALKING);
}

void Zombie :: performAttack(IGameplayMediator& mediator){
    if (isHypnotized) mediator.damageZombiesInArea(getAttackHitbox(), attackDamage, this);
    else {
        mediator.damagePlantInArea(getAttackHitbox(), attackDamage, this);
        mediator.playSound(GetRandomValue(0, 1) ? "CHOMP" : "CHOMP2", 0.7f);
    }
}

void Zombie :: updateCombat(float dt, IGameplayMediator& mediator){
    if (state == ZombieState::DYING || state == ZombieState::DEAD) return;
    onCustomCombat(dt, mediator);

    bool hasTarget = false;
    if (isHypnotized) {
        hasTarget = mediator.hasZombieInArea(getAttackHitbox(), this);
    } else {
        hasTarget = mediator.hasPlantInArea(getAttackHitbox());
    }

    setAttacking(hasTarget);
    
    if(!hasTarget){
        attackTimer = 0.0f;
        return;
    }

    float effectiveDt = dt;
    if (freezeTimer > 0.0f) effectiveDt = 0.0f;
    else if (chillTimer > 0.0f) effectiveDt = dt * 0.5f;

    attackTimer += effectiveDt;
    if (zombieData != nullptr && attackTimer >= zombieData -> getAttackInterval()){
        if (isHypnotized) mediator.damageZombiesInArea(getAttackHitbox(), attackDamage, this);
        else {
            mediator.damagePlantInArea(getAttackHitbox(), attackDamage, this);
            mediator.playSound(GetRandomValue(0, 1) ? "CHOMP" : "CHOMP2", 0.7f);
        }
        attackTimer = 0.0f;
    }
}

void Zombie::freeze(float duration) {
    freezeTimer = duration;
    chillTimer = std::max(chillTimer, duration + 5.0f); // Automatically chill for 5s after freeze
}

void Zombie::chill(float duration) {
    chillTimer = std::max(chillTimer, duration);
}


bool Zombie :: isAttacking() const { return state == ZombieState :: EATING; }

void Zombie :: onStateChanged(ZombieState newState) {
    switch(newState){
        case ZombieState :: WALKING:
            if (animation.hasClip("walk")) animation.playClip("walk");
            else if (animation.hasClip("anim_walk")) animation.playClip("anim_walk");
            animation.setLoopToggle(true);
            break;
        case ZombieState :: EATING:
            if (animation.hasClip("eat")) animation.playClip("eat");
            else if (animation.hasClip("anim_eat")) animation.playClip("anim_eat");
            animation.setLoopToggle(true);
            break;
        case ZombieState :: DYING:
            if (animation.hasClip("death")) {
                animation.playClip("death");
                animation.setLoopToggle(false);
            } else if (animation.hasClip("anim_death")) {
                animation.playClip("anim_death");
                animation.setLoopToggle(false);
            } else {
                setState(ZombieState::DEAD);
            }
            break;
        case ZombieState :: DEAD:
            break;
    }
}

