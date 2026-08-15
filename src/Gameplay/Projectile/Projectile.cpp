#include "Gameplay/Projectile/Projectile.hpp"

///////////////////////////////
///     PROJECTILE DATA     ///
///////////////////////////////
void ProjectileData::setRadius(float radius) {
    this -> radius = radius;
}
void ProjectileData::setRange(float range) {
    this -> range = range;
}
void ProjectileData::setVelocity(Vector2 velocity) {
    this -> velocity = velocity;
}
void ProjectileData::setTextureName(const std::string& textureName) {
    this -> textureName = textureName;
}

float ProjectileData::getRadius() const {
    return radius;
}
float ProjectileData::getRange() const { return range; }
Vector2 ProjectileData::getVelocity() const { return velocity; }
const std::string& ProjectileData::getTextureName() const { return textureName; }
float ProjectileData::getChillDuration() const { return chillDuration; }

void ProjectileData::setChillDuration(float duration) { chillDuration = duration; }
void ProjectileData::setLobbed(bool _lobbed) { lobbed = _lobbed; }
void ProjectileData::setSplashArea(Vector2 area) { splashArea = area; }
void ProjectileData::setScale(float _scale) { scale = _scale; }

bool ProjectileData::isLobbed() const { return lobbed; }
Vector2 ProjectileData::getSplashArea() const { return splashArea; }
float ProjectileData::getScale() const { return scale; }

/////////////////////////////////// 
///     PROJECTILE MECHANICS    ///
///////////////////////////////////

// Projectile :: Projectile(Vector2 pos, Vector2 vel, float _damage, float _radius, float _range, Texture2D* tex): 
//     damage(std :: max(0.0f, _damage)),
//     radius(std :: max(0.0f, _radius)),
//     range(std :: max(0.0f, _range)),
//     position(pos),
//     velocity(vel), texture(tex) {}

Projectile :: Projectile(Vector2 pos, ProjectileData* _projData, float _damage, float _range, Texture2D* tex)
    : projData(_projData), damage(std :: max(0.0f, _damage)), texture(tex)
{
    position = pos;
    
    if (projData) {
        velocity = projData -> getVelocity();
    }
}

void Projectile::setProjectileData(ProjectileData* data) {
    projData = data;
}

void Projectile :: setTexture(Texture2D* newTexture){
    texture = newTexture;
}

void Projectile::setReanimInstance(ReanimInstance anim) {
    reanim = std::move(anim);
    hasReanim = true;
}

bool Projectile::getHasReanim() const {
    return hasReanim;
}

void Projectile :: setDamage(float _d){
    damage = std :: max(0.0f, _d);
}
void Projectile :: setPosition(Vector2 newPos){
    position = newPos;
}
void Projectile::setVelocity(Vector2 newVel) { velocity = newVel; }
void Projectile::setType(ProjectileType t) { type = t; }

ProjectileType Projectile::getType(void) const { return type; }
float Projectile::getChillDuration(void) const { return projData ? projData->getChillDuration() : 0.0f; }

bool Projectile::isLobbed() const { return projData ? projData->isLobbed() : false; }
Vector2 Projectile::getSplashArea() const { return projData ? projData->getSplashArea() : Vector2{0.0f, 0.0f}; }
bool Projectile::hasImpacted() const { return impacted; }

void Projectile::setTarget(Vector2 target) {
    if (!isLobbed()) return;
    targetPos = target;
    impacted = false;
    
    // Parabolic trajectory math
    float distanceX = targetPos.x - position.x;
    if (velocity.x == 0.0f) velocity.x = 350.0f; // Prevent division by zero
    float timeToTarget = distanceX / velocity.x;
    
    if (timeToTarget <= 0.0f) timeToTarget = 0.1f;
    
    velocity.y = (targetPos.y - position.y - 0.5f * gravity * timeToTarget * timeToTarget) / timeToTarget;
}

float Projectile :: getDamage(void) const{
    return damage;
}
float Projectile :: getRadius(void) const{
    return projData ? projData -> getRadius() : 0.0f;
}
float Projectile :: getRange(void) const{
    return projData ? projData -> getRange() : 0.0f;
}
Vector2 Projectile :: getPosition(void) const{
    return position;
} 
Rectangle Projectile::getHitbox(void) const {
    float radius = getRadius();
    return Rectangle{position.x - radius, position.y - radius, 2.0f * radius, 2.0f * radius};
}

Rectangle Projectile::getCollisionHitbox(void) const {
    if (isLobbed() && !impacted) {
        return Rectangle{0, 0, 0, 0}; // Lobbed projectiles don't collide mid-air
    }

    Rectangle hitbox = getHitbox();
    // Normal projectiles can hit things slightly above/below them
    if (!isLobbed() && velocity.y != 0.0f) {
        hitbox.height *= 4.0f;
    }
    return hitbox;
}

Vector2 Projectile :: getVelocity(void) const{
    return velocity;
}

bool Projectile :: isDespawned(void) const{
    return despawned;
}
void Projectile :: Despawn(void){
    despawned = true;
}

void Projectile :: update(float dt){
    if(despawned || dt <= 0.0f || impacted) return;

    if (hasReanim) {
        reanim.updateTime(dt);
    }

    if (isLobbed()) {
        velocity.y += gravity * dt; // Apply gravity
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
        
        // If it passed the target X or reached the ground level (target Y)
        if (position.x >= targetPos.x || (velocity.y > 0 && position.y >= targetPos.y)) {
            impacted = true;
            position.y = targetPos.y; // Snap to ground
        }
    } else {
        Vector2 movement = {
            velocity.x * dt,
            velocity.y * dt
        };

        position.x += movement.x;
        position.y += movement.y;

        totalDistance += Vector2Length(movement);

        if(totalDistance >= getRange()){
            Despawn();
        }
    }
}

void Projectile :: draw(void) const{
    float scale = projData ? projData->getScale() : 1.0f;
    float r = getRadius() * scale;

    if (hasReanim) {
        Rectangle dst = {position.x - r - 15.0f, position.y - r - 10.0f, 2.0f * r + 30.0f, 2.0f * r + 20.0f};
        const_cast<ReanimInstance&>(reanim).draw(dst, WHITE);
        return;
    }

    if(texture == nullptr) return;

    Rectangle src = {0.0f, 0.0f, static_cast<float>(texture -> width), static_cast<float>(texture -> height)};
    Rectangle dst = {position.x - r, position.y - r, 2.0f * r, 2.0f * r};

    DrawTexturePro(*texture, src, dst, Vector2{0.0f, 0.0f}, 0, WHITE);
}
