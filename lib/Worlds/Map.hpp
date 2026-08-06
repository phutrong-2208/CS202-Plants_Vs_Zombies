#ifndef MAP_HPP
#define MAP_HPP

#include <Common.hpp>

class Map {
public:
    enum class State {SLIDING, CHOOSE_PLANTS, READY, PAUSED};

    virtual ~Map();

    virtual void update(float dt);
    virtual void draw() const;
    virtual void drawBackground() const;

    State getState() const;
    virtual bool isReady() const;
    bool isChoosingPlants() const;
    void finishChoosingPlants();
    // virtual Rectangle getGridArea() const;

protected:
    enum class SlidePhase {PAN_RIGHT, PAN_LEFT};

    State state = State :: SLIDING;
    State stateBeforePause = State :: READY;
    SlidePhase slidePhase = SlidePhase :: PAN_RIGHT;
    float slideTimer = 0.0f;
    float slideDuration = 2.0f;
    float focusX = 0.5f;
    Rectangle crop = {0, 0, 800, 600};
    Rectangle lawnSrc = {0, 0, 800, 600};
    Texture2D background = {};
    bool hasBackground = false;

    void loadBackground(const std :: string& path, Rectangle cropRect, Rectangle lawnRect);
    float getSlideProgress() const;
    float getSlidingCropX() const;
    Rectangle getViewportDest() const;
};

#endif
