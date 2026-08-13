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

// A clip layer renders a subset of tracks using its own clip time range.
// Used for multi-part plants (e.g. GatlingPea body + head) where body
// and head occupy non-overlapping segments on the same reanim timeline.
struct ClipLayer {
    float currentTime   = 0.0f;
    float clipLoopStart = -1.0f;
    float clipEnd       = 0.0f;
    bool  looping       = true;

    // Initial clip bounds — stored for resetToDefault()
    float initClipLoopStart = -1.0f;
    float initClipEnd       = 0.0f;
    float initStartTime     = 0.0f;

    std::unordered_set<std::string> showTracks;  // only render these tracks
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

    // Initial clip state — stored for resetToDefault()
    float initClipLoopStart = -1.0f;
    float initClipEnd       = 0.0f;
    float initStartTime     = 0.0f;

    ReanimParser*   rawAnim    = nullptr;
    TexturePackage* rawTexPack = nullptr;
    std::unordered_set<std::string> hiddenTracks;
    std::unordered_set<std::string> visibleTracks;
    bool useVisibleTrackFilter = false;

    // Additional clip layers for multi-part plants
    std::vector<ClipLayer> clipLayers;

public:
    // Setup
    void setAnimation(ReanimParser* Anim);
    void setTexturePackage(TexturePackage* TexPack);
    void setTextureScalar(float scaleFactor);
    void hideTrack(const std::string& trackName);
    void showOnlyTracks(const std::vector<std::string>& trackNames);
    Texture2D* getTrackTexture(const std::string& trackName) const;

    // ---------------------------------------------------------------
    // Clip layers — additional clips that render specific track subsets.
    // Used for multi-part plants where body and head tracks live on
    // non-overlapping timeline segments within the same reanim file.
    void addClipLayer(const std::string& clipName, const std::vector<std::string>& trackNames);

    // Returns a snapshot of every visible track at the given hitbox origin.
    // Used by ZombieDeathHandler to spawn parts at their real screen positions.
    std::vector<TrackSnapshot> getActiveTrackParts(Rectangle hitbox) const;

    // ---------------------------------------------------------------
    // Clip selection — call once after setAnimation().
    // clipName is without the "anim_" prefix, e.g. "idle", "shooting".
    // For the default idle loop, do NOT call playClip — just let the full
    // file loop naturally; the reanim file IS one complete idle cycle.
    bool playClip(const std::string& clipName);

    // Resets the primary clip and all clip layers to their initial states.
    // Used after a one-shot animation finishes to return to idle.
    void resetToDefault();

    void  updateTime(float deltaSeconds);
    void  setSpeed(float newSpeed);
    void  setLoopToggle(bool isTrue);
    float getCurrentTime() const;
    bool  isLooping() const;
    bool  isFinished() const;

    void draw(Rectangle hitbox, Color overrideTint = WHITE) const;
};

#endif
