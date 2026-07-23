#include "Gameplay/Projectile/ProjectileManager.hpp"

ProjectileManager :: ProjectileManager() = default;

void ProjectileManager :: addNew(std :: unique_ptr<Projectile> projectile){
    if(!projectile) return;
    projectiles.emplace_back(std :: move(projectile));
}
void ProjectileManager :: update(float dt){
    for(auto&prj : projectiles){
        if(!prj -> isDespawned()){ //hasn't been despawned
            prj -> update(dt);
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
