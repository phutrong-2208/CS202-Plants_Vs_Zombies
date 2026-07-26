#include "Gameplay/Projectile/Projectile.hpp"

///////////////////////////////
///     PROJECTILE DATA     ///
///////////////////////////////
void ProjectileData::setDamage(int damage) {
    this -> damage = damage;
}
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

/////////////////////////////////// 
///     PROJECTILE MECHANICS    ///
///////////////////////////////////

Projectile :: Projectile(Vector2 pos, Vector2 vel, int _damage, float _radius, float _range, Texture2D* tex): 
    damage(std :: max(0, _damage)),
    radius(std :: max(0.0f, _radius)),
    range(std :: max(0.0f, _range)),
    position(pos),
    velocity(vel), texture(tex) {}

void Projectile :: setTexture(Texture2D* newTexture){
    texture = newTexture;
}

void Projectile :: setDamage(int _d){
    damage = std :: max(0, _d);
}
void Projectile :: setPosition(Vector2 newPos){
    position = newPos;
}
void Projectile :: setVelocity(Vector2 veloc){
    velocity = veloc;
}

int Projectile :: getDamage(void) const{
    return damage;
}
float Projectile :: getRadius(void) const{
    return radius;
}
float Projectile :: getRange(void) const{
    return range;
}
Vector2 Projectile :: getPosition(void) const{
    return position;
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
    if(despawned || dt <= 0.0f) return;

    Vector2 movement = {
        velocity.x * dt,
        velocity.y * dt
    };

    position.x += movement.x;
    position.y += movement.y;

    totalDistance += Vector2Length(movement);

    if(totalDistance >= range){
        Despawn();
    }
}

void Projectile :: draw(void) const{
    if(texture == nullptr) return;

    Rectangle src = {0.0f, 0.0f, static_cast<float>(texture -> width), static_cast<float>(texture -> height)};
    Rectangle dst = {position.x, position.y, radius * 2.0f, radius * 2.0f};

    DrawTexturePro(*texture, src, dst, {static_cast<float>(radius), static_cast<float>(radius)}, 0, WHITE);
}
