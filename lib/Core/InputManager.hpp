#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <Common.hpp>

/// @brief Input Events
struct RawInputEvent {
    enum class InputType {
        EMPTY,
        LEFT_MOUSE_CLICKED,
        RIGHT_MOUSE_CLICKED,
        LEFT_MOUSE_RELEASED,
        RIGHT_MOUSE_RELEASED,
        LEFT_MOUSE_HOLD,
        RIGHT_MOUSE_HOLD,

        KEY_PRESSED,

        MOUSE_SCROLL,
    };

    InputType inputType = InputType::EMPTY;
    Vector2 position = {0, 0};
    KeyboardKey keySignal = KEY_NULL;
};

/// @brief Mouse Events
struct MouseState {
    float timePassed = 0.0f;
    bool pressed = false;
    Vector2 position = {0, 0};

    const float HOLD_TIME = 0.25f;
    
    void pressUpdate(std::queue <RawInputEvent>& inputQueue, bool isLeft);
    void releaseUpdate(std::queue <RawInputEvent>& inputQueue, bool isLeft);
};

class InputManager {
public:
    void update();

    void handleBackspace();
    
    bool hasEvents();
    RawInputEvent pollEvent();
private:
    std::queue <RawInputEvent> inputQueue;

    MouseState LeftMouse, RightMouse;
    float backspaceTime = 0.0f;
    const float BACKSPACE_DELAY = 0.5f;    // Wait half a second before double backspacing
    const float BACKSPACE_INTERVAL = 0.05f; // Then delete every 50ms
};
#endif