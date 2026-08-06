#ifndef DAYMAP_HPP
#define DAYMAP_HPP

#include "Worlds/Map.hpp"
#include <Core/AssetManager.hpp>
#include <Gameplay/Animation/ReanimInstance.hpp>

class DayMap : public Map {
private:
    Texture2D* sodTexture = nullptr;
    ReanimParser* sodRollData = nullptr;
    ReanimInstance sodRollAnimation;

    int grassLaneCount = 5;
    bool sodRollEnabled = false;
    bool sodRollFinished = true;
    float sodProgress = 1.0f;

    Vector2 getSodVirtualPosition() const;
    void drawSod() const;

public:
    DayMap(
        AssetManager* assetManager,
        int grassLanes,
        bool useSodRollIntro
    );

    void update(float dt) override;
    void drawBackground() const override;
    bool isReady() const override;
    // Rectangle getGridArea() const override;
};

#endif
