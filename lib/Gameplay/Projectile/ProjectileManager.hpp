#ifndef PROJECTILE_MANAGER_HPP
#define PROJECTILE_MANAGER_HPP

#include "Common.hpp"
#include <Gameplay/Projectile/Projectile.hpp>

class ProjectileManager {
private:
    std :: vector<std :: unique_ptr<Projectile>> projectiles;
public:
    ProjectileManager();
    ~ProjectileManager() = default;
    
    void addProjectile(std :: unique_ptr<Projectile> projectile);
    void update(float dt);
    void simulate(void) const;
}; 

#endif
