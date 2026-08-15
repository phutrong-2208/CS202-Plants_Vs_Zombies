#ifndef LAWNMOWER_HPP
#define LAWNMOWER_HPP

#include <Common.hpp>
#include <Core/TexturePackage.hpp>
#include <Core/ReanimParser.hpp>
#include <Gameplay/Animation/ReanimInstance.hpp>
#include <Gameplay/IGameplayMediator.hpp>

enum class LawnMowerState {
    READY,
    ROLLING,
    GONE
};

class LawnMower {
private:
    int lane = 0;
    Vector2 position = {0.0f, 0.0f};
    Vector2 startPosition = {0.0f, 0.0f};
    LawnMowerState state = LawnMowerState::READY;
    ReanimInstance reanim;
    float speed = 450.0f;
    float scalar = 0.85f;

public:
    LawnMower() = default;

    void init(int laneIndex, Vector2 startPos, TexturePackage* texPack, ReanimParser* animData);
    void reset();
    void update(float dt, IGameplayMediator* mediator);
    void draw() const;
    void trigger();

    LawnMowerState getState() const { return state; }
    bool isReady() const { return state == LawnMowerState::READY; }
    bool isRolling() const { return state == LawnMowerState::ROLLING; }
    bool isGone() const { return state == LawnMowerState::GONE; }
    int getLane() const { return lane; }
    Rectangle getHitbox() const;
    Rectangle getTriggerHitbox() const;
};

#endif
