#ifndef REANIMINSTANCE_HPP
#define REANIMINSTANCE_HPP

#include <Core/ReanimParser.hpp>
#include <Core/TextureManager.hpp>
#include <string>
#include <unordered_set>
#include <vector>

// A snapshot of a single track's render state at death-time.
struct TrackSnapshot {
    std::string trackName;
    Texture2D*  texture  = nullptr;
    float       worldX   = 0.0f;   // hitbox.x + frame.newX * scalar
    float       worldY   = 0.0f;   // hitbox.y + frame.newY * scalar
    float       scaleX   = 1.0f;
    float       scaleY   = 1.0f;
    float       skewX    = 0.0f;
    float       skewY    = 0.0f;
    float       alpha    = 1.0f;
};

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
    Texture2D* getTrackTexture(const std::string& trackName) const;

    // Returns a snapshot of every visible track at the given hitbox origin.
    // Used by ZombieDeathHandler to spawn parts at their real screen positions.
    std::vector<TrackSnapshot> getActiveTrackParts(Rectangle hitbox) const;

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
    bool  isFinished() const;

    void draw(Rectangle hitbox, Color overrideTint = WHITE) const;
};

#endif
