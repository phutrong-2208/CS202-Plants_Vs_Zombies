#ifndef ANIMATION_MANAGER_HPP
#define ANIMATION_MANAGER_HPP

#include "Core/ReanimParser.hpp"
#include "Core/TextureManager.hpp"
#include "Common.hpp"


class AnimationManager {
private:
    struct Clip {
        std :: string name;
        ReanimParser* animation;
    };

    std :: vector<Clip> clipList;
    int currentClip;
    float currentTime;
    float speed;
    bool looping;

public:
    AnimationManager();

    void addClip(const std :: string& name, ReanimParser* parser);

    void play(const std :: string& name);
    void play(int index);
    void stop();

    void setSpeed(float s){ 
        speed = s; 
    }
    void setLooping(bool l){ 
        looping = l; 
    }
    float getCurrentTime() const { 
        return currentTime; 
    }
    bool  isLooping() const { 
        return looping; 
    }
    void update(float dt);
    void draw(Vector2 position, TextureManager& texture, const std :: string& packName, float scaleRatio) const;
};

#endif