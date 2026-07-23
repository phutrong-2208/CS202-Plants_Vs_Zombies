#ifndef PROJECTILE_MANAGER_HPP
#define PROJECTILE_MANAGER_HPP

#include "Common.hpp"
#include "Projectile.hpp"

class ProjectileManager {
private:
    std :: vector<std :: unique_ptr<Projectile>> projectiles;
public:
    ProjectileManager();
    ~ProjectileManager() = default;
    
    void addNew(std :: unique_ptr<Projectile> projectile);
    void update(float dt);
    void simulate(void) const;
}; 

#endif
