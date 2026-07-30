#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include <Core/TextureManager.hpp>
#include <Core/AnimationManager.hpp>
#include <Gameplay/Animation/ReanimInstance.hpp>

enum ZombieType : int {
    NORMAL_ZOMBIE,
    DANCER_ZOMBIE,
    ZOMBIE_COUNT
};

class ZombieData {
private:
    float baseHealth = 100;
    float moveSpeed = 10.0f;
    float attackDamage = 20;

    float reanimScalar = 1.0f;
    std::string reanimPackage;
    std::string reanimAnim;
    std::string reanimClip;

public:
    ZombieData() = default;
    ZombieData(float health, float speed, int damage);

    int getBaseHealth() const;
    float getMoveSpeed() const;
    int getAttackDamage() const;
    float getReanimScalar() const;
    const std::string& getReanimPackage() const;
    const std::string& getReanimAnim() const;
    const std::string& getReanimClip() const;

    void setBaseHealth(float health);
    void setMoveSpeed(float speed);
    void setAttackDamage(int damage);
    void setReanimScalar(float scalar);
    void setReanimPackage(const std::string& package);
    void setReanimAnim(const std::string& animation);
    void setReanimClip(const std::string& clip);
};

class Zombie {
protected:
    float health = 0;
    float speed = 0.0f;
    int attackDamage = 0;
    bool attacking = false;
    Rectangle hitbox = {0.0f, 0.0f, 0.0f, 0.0f};
    ReanimInstance animation;
    ZombieData* zombieData = nullptr;

    void zombieSetup();

public:
    Zombie() = default;
    virtual ~Zombie() = default;

    virtual void updateTime(float dt);
    void setZombieData(ZombieData* data);
    void setReanimInstance(ReanimInstance anim);
    void draw();

    void receiveDamage(float damage);
    bool isDead() const;

    float getHealth() const;
    float getSpeed() const;
    int getAttackDamage() const;
    Rectangle getHitbox() const;

    void setHitbox(Rectangle newHitbox);
    void setAttacking(bool isAttacking);
    bool isAttacking() const;
};

#endif
