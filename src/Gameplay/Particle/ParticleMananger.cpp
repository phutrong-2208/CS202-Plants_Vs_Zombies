#include "Gameplay/Particle/ParticleManager.hpp"

void ParticleManager :: addParticle(std :: unique_ptr<Particle> particle){
    if(!particle) return;
    particles.emplace_back(std :: move(particle));
}

void ParticleManager :: update(float dt){
    for(auto& item : particles){
        item -> update(dt);
    }
    particles.erase(
        std :: remove_if(
            particles.begin(), particles.end(), 
            [](const auto& item){
                return item -> isExpired();
            }
        ),
        particles.end()
    );   
}

void ParticleManager :: draw(void) const{
    for(auto&item : particles){
        item -> draw();
    }
}

int ParticleManager :: handleClick(Vector2 mousePosition){
    for(auto item = particles.rbegin(); item != particles.rend(); ++item){
        const int value = (*item) -> collect(mousePosition);
        if(value > 0) return value;
    }
    return 0;
}



