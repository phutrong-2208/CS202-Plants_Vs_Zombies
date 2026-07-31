#ifndef REANIMINSTANCE_HPP
#define REANIMINSTANCE_HPP

#include <Core/ReanimParser.hpp>
#include <Core/TextureManager.hpp>
#include <string>
#include <unordered_set>
#include <vector>

class ReanimInstance {
private:
    float currentTime = 0.0f;
    float speed       = 1.0f;
    bool  looping     = true;
    float scalar      = 1.0f;

    // ---------------------------------------------------------------
    // Clip range — the sub-section of the timeline currently playing.
    //   clipLoopStart : where the clock rewinds when looping
    //   clipEnd       : when the clock wraps / clamps
    // Both are set by playClip().  If no clip has been selected,
    // updateTime falls back to [0, totalDuration].
    // ---------------------------------------------------------------
    float clipLoopStart = -1.0f;  // -1 = "no clip set"
    float clipEnd       =  0.0f;

    ReanimParser*   rawAnim    = nullptr;
    TexturePackage* rawTexPack = nullptr;
    std :: unordered_set<std :: string> hiddenTracks;
    std :: unordered_set<std :: string> visibleTracks;
    bool useVisibleTrackFilter = false;

public:
    // Setup
    void setAnimation(ReanimParser* Anim);
    void setTexturePackage(TexturePackage* TexPack);
    void setTextureScalar(float scaleFactor);
    void hideTrack(const std :: string& trackName);
    void showOnlyTracks(const std :: vector<std :: string>& trackNames);

    // ---------------------------------------------------------------
    // Clip selection — call once after setAnimation().
    // clipName is without the "anim_" prefix, e.g. "idle", "shooting".
    // Returns false if the clip was not found (keeps previous state).
    // ---------------------------------------------------------------
    bool playClip(const std :: string& clipName);

    void  updateTime(float deltaSeconds);
    void  setSpeed(float newSpeed);
    void  setLoopToggle(bool isTrue);
    float getCurrentTime() const;
    bool  isLooping() const;

    void draw(Rectangle hitbox);
};

#endif
