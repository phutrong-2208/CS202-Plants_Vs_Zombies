#pragma once
#include "Gameplay/Plants/ExplosivePlants/ExplosivePlant.hpp"

enum class SquashState {
    IDLE,
    LOOKING,
    JUMPING_UP,
    JUMPING_DOWN,
    SQUASHED,
    DONE
};

class Squash : public ExplosivePlant {
private:
    SquashState squashState = SquashState::IDLE;
    float stateTimer = 0.0f;
    float squashedLingerDuration = 0.8f;
    float jumpOffsetX = 0.0f;
    float jumpOffsetY = 0.0f;
    float startX = 0.0f;
    float targetX = 0.0f;
    float jumpProgress = 0.0f;
    float jumpUpDuration = 0.35f;
    float jumpDownDuration = 0.18f;
    bool damageDealt = false;
    IGameplayMediator* lastMediator = nullptr;

public:
    PlantType getType() override;
    void updateTime(float deltaSeconds) override;
    void performAction(IGameplayMediator* mediator) override;
    void draw(Rectangle hitbox) override;
};
