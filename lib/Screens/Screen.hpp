#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <Core/InputManager.hpp>

class Screen {
public:
    virtual ~Screen() = default;

    virtual void update(float dt) = 0;
    virtual void draw() = 0;
    virtual void handleInput(const RawInputEvent& inputEvent) = 0;
};

#endif
