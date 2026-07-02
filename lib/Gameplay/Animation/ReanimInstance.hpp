#ifndef REANIMINSTANCE_HPP
#define REANIMINSTANCE_HPP

#include <Core/ReanimParser.hpp>
#include <Core/TextureManager.hpp>

class ReanimInstance {
private:
    float currentTime = 0.0f;
    float speed = 1.0f;
    bool looping = true;
    ReanimParser* rawAnim = nullptr;
    TexturePackage* rawTexPack = nullptr;
public:
    void setAnimation(ReanimParser* Anim);
    void setTexturePackage(TexturePackage* TexPack);

    void updateTime(float deltaSeconds);
    void setSpeed(float newSpeed);
    void setLoopToggle(bool isTrue);
    float getCurrentTime() const;
    bool isLooping() const;

    void draw(Rectangle hitbox);
};

#endif