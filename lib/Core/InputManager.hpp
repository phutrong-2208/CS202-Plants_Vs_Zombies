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

        KEY_PRESSED,

        MOUSE_SCROLL,
    };

    InputType inputType = InputType::EMPTY;
    Vector2 position = {0, 0};
    KeyboardKey keySignal = KEY_NULL;
};

class InputManager {
public:
    void update();

    void handleBackspace();
    
    bool hasEvents();
    RawInputEvent pollEvent();
private:
    std::queue <RawInputEvent> inputQueue;

    float backspaceTime = 0.0f;
    const float BACKSPACE_DELAY = 0.5f;    // Wait half a second before double backspacing
    const float BACKSPACE_INTERVAL = 0.05f; // Then delete every 50ms
};
#endif