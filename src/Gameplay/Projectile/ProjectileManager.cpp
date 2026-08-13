#include "Gameplay/Projectile/ProjectileManager.hpp"

void ProjectileManager::setMediator(IGameplayMediator* mediator) {
    gameplayMediator = mediator;
}

void ProjectileManager :: addProjectile(std :: unique_ptr<Projectile> projectile){
    if(!projectile) return;
    projectiles.emplace_back(std :: move(projectile));
}
void ProjectileManager :: update(float dt){
    for(auto&prj : projectiles){
        if(!prj -> isDespawned()){ //hasn't been despawned
            prj -> update(dt);
        }
    }    
}
void ProjectileManager::toggleProjectiles() {
    for(auto&prj : projectiles){
        if (prj -> isDespawned()) continue;

        if (prj->isLobbed() && prj->hasImpacted()) {
            if (gameplayMediator) {
                gameplayMediator->explodeProjectile(prj.get());
            }
            prj->Despawn();
        } else if (gameplayMediator && gameplayMediator -> touchTarget(prj.get())) {
            prj -> Despawn();
        }
    }    
    
    projectiles.erase(
        std :: remove_if(projectiles.begin(), projectiles.end(), 
            [](const std :: unique_ptr<Projectile>& prj){
                return prj -> isDespawned();
            }
        ), 
        projectiles.end()
    );
}

void ProjectileManager :: simulate(void) const{
    for(auto&prj : projectiles){
        if(!prj -> isDespawned()){
            prj -> draw();
        }
    }
}
