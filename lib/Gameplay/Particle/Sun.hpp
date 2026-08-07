#ifndef SUN_HPP
#define SUN_HPP

#include "Common.hpp"
#include "Gameplay/Animation/ReanimInstance.hpp"
#include "Gameplay/Particle/Particle.hpp"

enum class SunState{
    DROPPING, 
    IDLE, 
    COLLECTING
};

class Sun : public Particle{
private:
    static const int SUN_NORMAL_GAIN = 25;
    int value = SUN_NORMAL_GAIN;
    float targetY = 0.0f;
    bool collected = false;
    SunState state = SunState :: DROPPING;
    ReanimInstance animation;
public:
    Sun(
        ReanimInstance animation,
        Vector2 position,
        float targetY,
        int value = SUN_NORMAL_GAIN
    );

    void update(float dt) override;
    void draw() const override;
    int collect(Vector2 mousePosition) override;
    Rectangle getHitBox() const override;
};

#endif
