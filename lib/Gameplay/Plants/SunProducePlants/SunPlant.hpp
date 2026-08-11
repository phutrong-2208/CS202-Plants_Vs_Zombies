#ifndef SUNPLANT_HPP
#define SUNPLANT_HPP

#include <Gameplay/Plants/Plant.hpp>

class SunPlant : public Plant {
protected:
    float blushTimer = 0.0f;
public:
    void triggerBlush(float duration = 1.0f);
    void updateTime(float deltaSeconds) override;
    void draw(Rectangle hitbox) override;
};

#endif