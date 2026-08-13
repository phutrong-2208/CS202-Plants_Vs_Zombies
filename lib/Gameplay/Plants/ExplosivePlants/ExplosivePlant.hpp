#ifndef EXPLOSIVEPLANT_HPP
#define EXPLOSIVEPLANT_HPP

#include "Gameplay/Plants/Plant.hpp"

class ExplosivePlant : public Plant {
protected:
    bool readyToExplode = false;
    bool animationStarted = false;

public:
    ExplosivePlant();
    virtual ~ExplosivePlant() = default;

    virtual void onActionAnimationFinished() override;
    virtual void performAction(IGameplayMediator* mediator) override;
};

#endif
