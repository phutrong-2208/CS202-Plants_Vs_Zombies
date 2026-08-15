#ifndef REANIM_PARTICLE_HPP
#define REANIM_PARTICLE_HPP

#include "Common.hpp"
#include "Gameplay/Animation/ReanimInstance.hpp"
#include "Gameplay/Particle/Particle.hpp"

class ReanimParticle : public Particle {
private:
    ReanimInstance animation;
    Rectangle drawBounds;
    float startDelay = 0.0f;
    float animSpeed = 1.0f;
    Color tint = WHITE;

public:
    ReanimParticle(
        ReanimInstance anim,
        Vector2 pos,
        Rectangle bounds,
        float duration = 1.0f,
        const std::string& clip = "",
        float delay = 0.0f,
        float speed = 1.0f,
        Color drawTint = WHITE,
        bool loop = false
    ) : Particle(nullptr, pos, {}, {}, duration, 1.0f),
        animation(std::move(anim)),
        drawBounds(bounds),
        startDelay(delay),
        animSpeed(speed),
        tint(drawTint)
    {
        if (!clip.empty()) {
            animation.playClip(clip);
        }
        animation.setLoopToggle(loop);
    }

    void update(float dt) override {
        if (expired) return;
        
        if (startDelay > 0.0f) {
            startDelay -= dt;
            return;
        }

        lifeTime -= dt;
        animation.updateTime(dt * animSpeed);
        if (lifeTime <= 0.0f || (!animation.isLooping() && animation.isFinished())) {
            expired = true;
        }
    }

    void draw() const override {
        if (expired || startDelay > 0.0f) return;
        const_cast<ReanimInstance&>(animation).draw(drawBounds, tint);
    }
};

#endif
