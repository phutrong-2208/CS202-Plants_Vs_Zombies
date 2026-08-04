#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include <Core/TextureManager.hpp>
#include <Core/AnimationManager.hpp>
#include <Gameplay/Animation/ReanimInstance.hpp>
#include <Gameplay/Zombies/ZombieType.hpp>
#include <Gameplay/IGameplayMediator.hpp>

enum class ZombieState {
    WALKING,
    EATING,
    DYING,
    DEAD
};

class ZombieData {
private:
    float baseHealth = 100;
    float armorHealth = 0;
    float alternateHealth = 0;
    float moveSpeed = 10.0f;
    float attackDamage = 20;
    float attackInterval = 1.0f;
    int unitCount = 1;

    float reanimScalar = 1.0f;
    std::string reanimPackage;
    std::string reanimAnim;
    std::string reanimClip;
    std::vector<std::string> hiddenTracks;

public:
    ZombieData() = default;
    ZombieData(float health, float speed, int damage);

    int getBaseHealth() const;
    int getArmorHealth() const;
    int getAlternateHealth() const;
    float getMoveSpeed() const;
    int getAttackDamage() const;
    float getAttackInterval() const;
    int getUnitCount() const;
    float getReanimScalar() const;
    const std::string& getReanimPackage() const;
    const std::string& getReanimAnim() const;
    const std::string& getReanimClip() const;
    const std::vector<std::string>& getHiddenTracks() const;

    void setBaseHealth(float health);
    void setArmorHealth(float health);
    void setAlternateHealth(float health);
    void setMoveSpeed(float speed);
    void setAttackDamage(int damage);
    void setAttackInterval(float interval);
    void setUnitCount(int count);
    void setReanimScalar(float scalar);
    void setReanimPackage(const std::string& package);
    void setReanimAnim(const std::string& animation);
    void setReanimClip(const std::string& clip);
    void setHiddenTracks(std::vector<std::string> tracks);
};


class Zombie {
protected:
    float health = 0;
    float speed = 0.0f;
    int attackDamage = 0;

    float attackTimer = 0.0f;
    float deathTimer = 0.0f;

    ZombieState state = ZombieState :: WALKING;
    Rectangle hitbox = {0.0f, 0.0f, 0.0f, 0.0f};
    ReanimInstance animation;
    ZombieData* zombieData = nullptr;

    void zombieSetup();
    virtual void onStateChanged(ZombieState newState);

public:
    Zombie() = default;
    virtual ~Zombie() = default;

    virtual void updateTime(float dt);
    void setZombieData(ZombieData* data);
    void setReanimInstance(ReanimInstance anim);
    void draw();

    void receiveDamage(float damage, IGameplayMediator* mediator = nullptr);
    bool isDead() const;

    float getHealth() const;
    float getSpeed() const;
    int getAttackDamage() const;
    Rectangle getHitbox() const;
    virtual Rectangle getAttackHitbox() const;
    ZombieState getState() const;

    void setHitbox(Rectangle newHitbox);
    void setState(ZombieState newState);
    void setAttacking(bool isAttacking);
    bool isAttacking() const;
    
    void performAttack(IGameplayMediator& mediator);
    void updateCombat(float dt, IGameplayMediator& mediator);
};

#endif
