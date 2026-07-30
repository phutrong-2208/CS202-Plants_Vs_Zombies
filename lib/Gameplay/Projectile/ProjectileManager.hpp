#ifndef PROJECTILE_MANAGER_HPP
#define PROJECTILE_MANAGER_HPP

#include "Common.hpp"
#include <Gameplay/IGameplayMediator.hpp>
#include <Gameplay/Projectile/Projectile.hpp>

class ProjectileManager {
private:
    std :: vector<std :: unique_ptr<Projectile>> projectiles;
    IGameplayMediator* gameplayMediator = nullptr;
public:
    ProjectileManager() = default;
    ~ProjectileManager() = default;
    void setMediator(IGameplayMediator* mediator);
    
    void addProjectile(std :: unique_ptr<Projectile> projectile);
    void update(float dt);
    void toggleProjectiles();
    void simulate(void) const;
}; 

#endif
