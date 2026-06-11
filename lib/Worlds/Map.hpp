#ifndef MAP_HPP
#define MAP_HPP

#include <Core/TextureManager.hpp>

class Map {
public:
    enum class State { SLIDING, READY };

    virtual ~Map() = default;

    virtual void update(float dt);
    virtual void draw(TextureManager* file) const = 0;

    bool isReady() const;
    virtual Rectangle getGridArea() const;

protected:
    State state = State::SLIDING;
    float slideTimer = 0.0f;
    float slideDuration = 3.0f;
    float focusX = 0.5f;

    float getSlideProgress() const;
};

#endif
