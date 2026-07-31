#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <Core/AssetManager.hpp>
#include <Core/InputManager.hpp>

class Screen {
protected:
    AssetManager* assetManager = nullptr;
public:
    virtual ~Screen() = default;

    virtual void update(float dt) = 0;
    virtual void draw() = 0;
    virtual void handleInput(const RawInputEvent& inputEvent) = 0;

    void setAssetManager(AssetManager* manager);
};

#endif
