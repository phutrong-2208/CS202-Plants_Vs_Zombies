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
float ProjectileData::getRange() const {
    return range;
}
Vector2 ProjectileData::getVelocity() const {
    return velocity;
}
const std::string& ProjectileData::getTextureName() const {
    return textureName;
}

/////////////////////////////////// 
///     PROJECTILE MECHANICS    ///
///////////////////////////////////

Projectile :: Projectile(Vector2 pos, Vector2 vel, float _damage, float _radius, float _range, Texture2D* tex): 
    damage(std :: max(0.0f, _damage)),
    radius(std :: max(0.0f, _radius)),
    range(std :: max(0.0f, _range)),
    position(pos),
    velocity(vel), texture(tex) {}

Projectile :: Projectile(Vector2 pos, ProjectileData* _projData, float _damage, float _range, Texture2D* tex)
    : projData(_projData), damage(std :: max(0.0f, _damage)), range(std :: max(0.0f, _range)), texture(tex)
{
    position = pos;
    
    if (projData) {
        velocity = projData -> getVelocity();
        radius   = std :: max(0.0f, projData -> getRadius());
    }
}

void Projectile :: setTexture(Texture2D* newTexture){
    texture = newTexture;
}

void Projectile :: setDamage(float _d){
    damage = std :: max(0.0f, _d);
}
void Projectile :: setPosition(Vector2 newPos){
    position = newPos;
}
void Projectile :: setVelocity(Vector2 veloc){
    velocity = veloc;
}

float Projectile :: getDamage(void) const{
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
Rectangle Projectile::getHitbox(void) const {
    return Rectangle{position.x - radius, position.y - radius, 2.0f * radius, 2.0f * radius};
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
    Rectangle dst = getHitbox();

    DrawTexturePro(*texture, src, dst, Vector2{0.0f, 0.0f}, 0, WHITE);
}
