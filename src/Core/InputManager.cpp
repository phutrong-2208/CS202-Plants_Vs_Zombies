#include <Core/InputManager.hpp>

/// @brief The function handles the doubling backspace with a considerable delay between
void InputManager::handleBackspace() {
    RawInputEvent newInput;
    newInput.inputType = RawInputEvent::InputType::KEY_PRESSED;
    newInput.keySignal = KEY_BACKSPACE;

    if (IsKeyPressed(KEY_BACKSPACE)) {
        inputQueue.push(newInput);
        return;
    }

    if (IsKeyDown(KEY_BACKSPACE)) {
        backspaceTime += GetFrameTime();
        
        if (backspaceTime >= BACKSPACE_DELAY + BACKSPACE_INTERVAL) {
            inputQueue.push(newInput);
            inputQueue.push(newInput);
            backspaceTime = BACKSPACE_DELAY;
        }
    } else {
        backspaceTime = 0.0f;
    }
}

/// @brief The function to handle simple input updates: from the keyboard, from the mouse
void InputManager::update() {
    KeyboardKey keyGet = (KeyboardKey) GetKeyPressed();
    if (keyGet != KEY_NULL && keyGet != KEY_BACKSPACE) {
        RawInputEvent newInput;
        newInput.inputType = RawInputEvent::InputType::KEY_PRESSED;
        newInput.keySignal = keyGet;

        inputQueue.push(newInput);
    }

    handleBackspace();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        RawInputEvent leftClick;
        leftClick.inputType = RawInputEvent::InputType::LEFT_MOUSE_CLICKED;
        leftClick.position = GetMousePosition();
        inputQueue.push(leftClick);
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        RawInputEvent leftRelease;
        leftRelease.inputType = RawInputEvent::InputType::LEFT_MOUSE_RELEASED;
        leftRelease.position = GetMousePosition();
        inputQueue.push(leftRelease);
    }

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        RawInputEvent rightClick;
        rightClick.inputType = RawInputEvent::InputType::RIGHT_MOUSE_CLICKED;
        rightClick.position = GetMousePosition();
        inputQueue.push(rightClick);
    }
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
        RawInputEvent rightRelease;
        rightRelease.inputType = RawInputEvent::InputType::RIGHT_MOUSE_RELEASED;
        rightRelease.position = GetMousePosition();
        inputQueue.push(rightRelease);
    }
}

bool InputManager::hasEvents() {
    return !inputQueue.empty();
}

RawInputEvent InputManager::pollEvent() {
    if (inputQueue.empty()) {
        return RawInputEvent();
    }

    RawInputEvent event = inputQueue.front();
    inputQueue.pop();
    return event;
}