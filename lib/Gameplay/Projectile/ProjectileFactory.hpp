#ifndef PROJECTILEFACTORY_HPP
#define PROJECTILEFACTORY_HPP

#include <Gameplay/Projectile/Projectile.hpp>
#include <Core/TexturePackage.hpp>

class ProjectileDataset {
private:
    std::map <ProjectileType, std::unique_ptr<ProjectileData>> projectileDatasetMap;
    void loadFromFile(const std::string& filepath);
public:
    ProjectileDataset();
    ProjectileData* getProjectileData(ProjectileType pType);
};

class ProjectileFactory {
private:
    TexturePackage* projTexturePackage = nullptr;
    std::unique_ptr <ProjectileDataset> projectileDataStorage = nullptr;

public:
    void setTexturePackage(TexturePackage* package);
    void loadProjectileMechanics();

    // Get the shared ProjectileData blueprint for a given type
    ProjectileData* getProjectileData(ProjectileType pType);

    // Resolve the texture for a given projectile type from the texture package
    Texture2D* getProjectileTexture(ProjectileType pType);

    std::unique_ptr <Projectile> createProjectile(ProjectileType pType, Vector2 spawnPos, float damage, float range);
};
#endif