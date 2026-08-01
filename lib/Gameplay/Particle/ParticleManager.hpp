#ifndef PARTICLE_MANAGER_HPP
#define PARTICLE_MANAGER_HPP

#include "Common.hpp"
#include "Gameplay/Particle/Particle.hpp"

class ParticleManager{
private:
    std :: vector<std :: unique_ptr<Particle>> particles;
public:
    void addParticle(std :: unique_ptr<Particle> particle);

    void update(float dt);
    void draw(void) const;

    int handleClick(Vector2 position);
};  


#endif
