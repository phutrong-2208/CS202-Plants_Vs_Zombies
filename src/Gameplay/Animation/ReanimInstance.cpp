#include <Gameplay/Animation/ReanimInstance.hpp>
#include <rlgl.h>

void ReanimInstance::setAnimation(ReanimParser* Anim) {
    rawAnim       = Anim;
    clipLoopStart = -1.0f;
    clipEnd       =  0.0f;
    currentTime   =  0.0f;
}

void ReanimInstance::setTexturePackage(TexturePackage* TexPack) {
    rawTexPack = TexPack;
}

void ReanimInstance::setTextureScalar(float scaleFactor) {
    scalar = scaleFactor;
}

void ReanimInstance::hideTrack(const std::string& trackName) {
    hiddenTracks.insert(trackName);
}

void ReanimInstance::showOnlyTracks(const std::vector<std::string>& trackNames) {
    visibleTracks.clear();
    visibleTracks.insert(trackNames.begin(), trackNames.end());
    useVisibleTrackFilter = true;
}

// Clip selection (optional — call this when you want to play a specific
// named animation segment, e.g. "shooting", "die".
// For the default idle loop, do NOT call playClip — just let the full
// file loop naturally; the reanim file IS one complete idle cycle.

Texture2D* ReanimInstance::getTrackTexture(const std::string& trackName) const {
    if (hiddenTracks.find(trackName) != hiddenTracks.end()) return nullptr;
    if (useVisibleTrackFilter && visibleTracks.find(trackName) == visibleTracks.end()) return nullptr;

    if (rawAnim == nullptr || rawTexPack == nullptr) return nullptr;
    const ReanimTrack* track = rawAnim->getTrack(trackName);
    if (track == nullptr) return nullptr;
    
    const float loopStart = (clipLoopStart >= 0.0f) ? clipLoopStart : rawAnim->getLoopStartTime();
    const float timelineEnd = (clipLoopStart >= 0.0f) ? clipEnd : rawAnim->getDuration();
    Frame frame = track->getInterpolatedFrame(currentTime, loopStart, timelineEnd);
    return rawTexPack->GetTexture(frame.getTextureKey());
}

bool ReanimInstance::setDefaultClip(const std::string& clipName) {
    if (playClip(clipName)) {
        initClipLoopStart = clipLoopStart;
        initClipEnd       = clipEnd;
        initStartTime     = currentTime;
        return true;
    }
    return false;
}

bool ReanimInstance::playClip(const std::string& clipName) {
    if (rawAnim == nullptr) return false;
    const AnimClip* clip = rawAnim->getClip(clipName);
    if (clip == nullptr) {
        TraceLog(LOG_WARNING, "ReanimInstance: clip '%s' not found", clipName.c_str());
        return false;
    }

    currentTime   = clip->startTime;
    clipLoopStart = clip->loopStart;
    clipEnd       = clip->endTime;
    looping       = false; 

    return true;
}

bool ReanimInstance::playClipLayer(const std::string& clipName, int layerIndex) {
    if (rawAnim == nullptr || layerIndex < 0 || layerIndex >= (int)clipLayers.size()) return false;
    
    const AnimClip* clip = rawAnim->getClip(clipName);
    if (clip == nullptr) {
        TraceLog(LOG_WARNING, "ReanimInstance::playClipLayer: clip '%s' not found", clipName.c_str());
        return false;
    }
    
    clipLayers[layerIndex].currentTime   = clip->startTime;
    clipLayers[layerIndex].clipLoopStart = clip->loopStart;
    clipLayers[layerIndex].clipEnd       = clip->endTime;
    clipLayers[layerIndex].looping       = false; 
    
    return true;
}

void ReanimInstance::addClipLayer(const std::string& clipName, const std::vector<std::string>& trackNames) {
    if (rawAnim == nullptr) return;

    const AnimClip* clip = rawAnim->getClip(clipName);

    if (clip == nullptr) {
        TraceLog(LOG_WARNING, "ReanimInstance::addClipLayer: clip '%s' not found", clipName.c_str());
        return;
    }

    ClipLayer layer;
    layer.currentTime   = clip->startTime;
    layer.clipLoopStart = clip->loopStart;
    layer.clipEnd       = clip->endTime;
    layer.looping       = true;

    // Store initial state for resetToDefault()
    layer.initClipLoopStart = layer.clipLoopStart;
    layer.initClipEnd       = layer.clipEnd;
    layer.initStartTime     = layer.currentTime;

    layer.showTracks.insert(trackNames.begin(), trackNames.end());
    clipLayers.push_back(std::move(layer));
}

void ReanimInstance::resetToDefault() {
    // Restore primary clip to initial state
    clipLoopStart = initClipLoopStart;
    clipEnd       = initClipEnd;
    currentTime   = initStartTime;
    looping       = true;

    // Restore all clip layers to initial state
    for (auto& layer : clipLayers) {
        layer.clipLoopStart = layer.initClipLoopStart;
        layer.clipEnd       = layer.initClipEnd;
        layer.currentTime   = layer.initStartTime;
        layer.looping       = true;
    }
}

// -----------------------------------------------------------------------
// Playback
// -----------------------------------------------------------------------

// Helper: advance a single time value within a clip range
static void advanceClipTime(float& time, float deltaSpeed,
                            float loopStart, float end,
                            bool isLooping, float fullDuration, float globalLoopStart) {
    time += deltaSpeed;

    if (loopStart >= 0.0f) {
        if (isLooping && end > loopStart && time >= end) {
            float range = end - loopStart;
            time = loopStart + std::fmod(time - loopStart, range);
        }
        if (!isLooping) {
            time = std::min(time, end);
        }
    } else {
        if (isLooping && time >= fullDuration) {
            time = std::fmod(time, fullDuration);
        }
        if (!isLooping) {
            time = std::min(time, fullDuration);
        }
    }
}

void ReanimInstance::updateTime(float deltaSeconds) {
    if (rawAnim == nullptr) return;

    const float delta = deltaSeconds * speed;
    const float duration = rawAnim->getDuration();
    const float globalLoopStart = rawAnim->getLoopStartTime();

    // Advance primary clip
    advanceClipTime(currentTime, delta, clipLoopStart, clipEnd,
                    looping, duration, globalLoopStart);

    // Advance all clip layers
    for (auto& layer : clipLayers) {
        advanceClipTime(layer.currentTime, delta,
                        layer.clipLoopStart, layer.clipEnd,
                        layer.looping, duration, globalLoopStart);
        
        // Auto-revert if layer was playing a non-looping clip and has reached the end
        if (!layer.looping && layer.currentTime >= layer.clipEnd) {
            layer.clipLoopStart = layer.initClipLoopStart;
            layer.clipEnd       = layer.initClipEnd;
            layer.currentTime   = layer.initStartTime;
            layer.looping       = true;
        }
    }
}

void ReanimInstance::setSpeed(float newSpeed) {
    speed = newSpeed;
}
void ReanimInstance::setLoopToggle(bool isTrue) {
    looping = isTrue;
}
float ReanimInstance::getCurrentTime() const {
    return currentTime;
}
bool ReanimInstance::isLooping() const {
    return looping;
}

bool ReanimInstance::isFinished() const {
    if(looping || rawAnim == nullptr) return false;

    const float timelineEnd = (clipLoopStart >= 0.0f) ? clipEnd : rawAnim->getDuration();
    return currentTime >= timelineEnd;
}

// -----------------------------------------------------------------------
// Rendering helpers
// -----------------------------------------------------------------------

// Draw a single track at the given time using the given clip bounds.
static void drawTrack(const ReanimTrack* track, float time,
                      float loopStart, float timelineEnd,
                      float scalar, Rectangle hitbox,
                      TexturePackage* texPack, Color overrideTint) {
    Frame frame = track->getInterpolatedFrame(time, loopStart, timelineEnd);
    if (frame.alpha <= 0.0f) return;

    Texture2D* currentTex = texPack->GetTexture(frame.getTextureKey());
    if (currentTex == nullptr) return;

    const float kx = frame.skewX * DEG2RAD;
    const float ky = frame.skewY * DEG2RAD;

    const float a  =  frame.scaleX * cosf(kx) * scalar;
    const float b_ =  frame.scaleX * sinf(kx) * scalar;
    const float c  = -frame.scaleY * sinf(ky) * scalar;
    const float d  =  frame.scaleY * cosf(ky) * scalar;
    const float tx =  hitbox.x + frame.newX * scalar;
    const float ty =  hitbox.y + frame.newY * scalar;

    const float mat[16] = {
        a,    b_,   0.0f, 0.0f,
        c,    d,    0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        tx,   ty,   0.0f, 1.0f
    };

    rlPushMatrix();
    rlMultMatrixf(mat);

    const float w = (float)currentTex->width;
    const float h = (float)currentTex->height;

    const unsigned char alphaByte = (unsigned char)(frame.alpha * overrideTint.a);
    const Color tint = {overrideTint.r, overrideTint.g, overrideTint.b, alphaByte};

    DrawTexturePro(
        *currentTex,
        {0, 0, w, h},  
        {0, 0, w, h},
        {0, 0},
        0.0f,
        tint
    );

    rlPopMatrix();
}

// -----------------------------------------------------------------------
// Main draw
// -----------------------------------------------------------------------

void ReanimInstance::draw(Rectangle hitbox, Color overrideTint) const {
    if (rawAnim == nullptr || rawTexPack == nullptr) {
        TraceLog(LOG_ERROR, "ReanimInstance: animation or texture package not set");
        return;
    }

    const float loopStart = (clipLoopStart >= 0.0f) ? clipLoopStart : rawAnim->getLoopStartTime();
    const float timelineEnd = (clipLoopStart >= 0.0f) ? clipEnd : rawAnim->getDuration();
    const int   trackCount = rawAnim->getTrackCount();

    // --- Pass 1: Render primary clip (body tracks) ---
    for (int i = 0; i < trackCount; ++i) {
        const ReanimTrack* track = rawAnim->getTrack(i);
        if (track == nullptr) continue;

        const std::string trackName = track->getTrackName();
        if (hiddenTracks.find(trackName) != hiddenTracks.end()) continue;
        if (useVisibleTrackFilter && visibleTracks.find(trackName) == visibleTracks.end()) continue;

        drawTrack(track, currentTime, loopStart, timelineEnd,
                  scalar, hitbox, rawTexPack, overrideTint);
    }

    // --- Pass 2: Render each clip layer (head tracks, etc.) ---
    for (const auto& layer : clipLayers) {
        const float layerLoopStart = (layer.clipLoopStart >= 0.0f)
                                     ? layer.clipLoopStart : rawAnim->getLoopStartTime();
        const float layerEnd = (layer.clipLoopStart >= 0.0f)
                               ? layer.clipEnd : rawAnim->getDuration();

        for (int i = 0; i < trackCount; ++i) {
            const ReanimTrack* track = rawAnim->getTrack(i);
            if (track == nullptr) continue;

            const std::string& trackName = track->getTrackName();
            // Only render tracks that belong to this layer
            if (layer.showTracks.find(trackName) == layer.showTracks.end()) continue;

            drawTrack(track, layer.currentTime, layerLoopStart, layerEnd,
                      scalar, hitbox, rawTexPack, overrideTint);
        }
    }
}

std::vector<TrackSnapshot> ReanimInstance::getActiveTrackParts(Rectangle hitbox) const {
    std::vector<TrackSnapshot> result;
    if (rawAnim == nullptr || rawTexPack == nullptr) return result;

    const float loopStart   = (clipLoopStart >= 0.0f) ? clipLoopStart : rawAnim->getLoopStartTime();
    const float timelineEnd = (clipLoopStart >= 0.0f) ? clipEnd       : rawAnim->getDuration();
    const int   trackCount  = rawAnim->getTrackCount();

    // Primary clip tracks
    for (int i = 0; i < trackCount; ++i) {
        const ReanimTrack* track = rawAnim->getTrack(i);
        if (track == nullptr) continue;

        const std::string& trackName = track->getTrackName();
        if (hiddenTracks.find(trackName) != hiddenTracks.end()) continue;
        if (useVisibleTrackFilter && visibleTracks.find(trackName) == visibleTracks.end()) continue;

        Frame frame = track->getInterpolatedFrame(currentTime, loopStart, timelineEnd);
        if (frame.alpha <= 0.0f) continue;

        Texture2D* tex = rawTexPack->GetTexture(frame.getTextureKey());
        if (tex == nullptr) continue;

        TrackSnapshot snap;
        snap.trackName = trackName;
        snap.texture   = tex;
        snap.worldX    = hitbox.x + frame.newX * scalar;
        snap.worldY    = hitbox.y + frame.newY * scalar;
        snap.scaleX    = frame.scaleX * scalar;
        snap.scaleY    = frame.scaleY * scalar;
        snap.skewX     = frame.skewX;
        snap.skewY     = frame.skewY;
        snap.alpha     = frame.alpha;
        result.push_back(snap);
    }

    // Clip layer tracks
    for (const auto& layer : clipLayers) {
        const float layerLoopStart = (layer.clipLoopStart >= 0.0f)
                                     ? layer.clipLoopStart : rawAnim->getLoopStartTime();
        const float layerEnd = (layer.clipLoopStart >= 0.0f)
                               ? layer.clipEnd : rawAnim->getDuration();

        for (int i = 0; i < trackCount; ++i) {
            const ReanimTrack* track = rawAnim->getTrack(i);
            if (track == nullptr) continue;

            const std::string& trackName = track->getTrackName();
            if (layer.showTracks.find(trackName) == layer.showTracks.end()) continue;


            Frame frame = track->getInterpolatedFrame(layer.currentTime, layerLoopStart, layerEnd);
            if (frame.alpha <= 0.0f) continue;

            Texture2D* tex = rawTexPack->GetTexture(frame.getTextureKey());
            if (tex == nullptr) continue;

            TrackSnapshot snap;
            snap.trackName = trackName;
            snap.texture   = tex;
            snap.worldX    = hitbox.x + frame.newX * scalar;
            snap.worldY    = hitbox.y + frame.newY * scalar;
            snap.scaleX    = frame.scaleX * scalar;
            snap.scaleY    = frame.scaleY * scalar;
            snap.skewX     = frame.skewX;
            snap.skewY     = frame.skewY;
            snap.alpha     = frame.alpha;
            result.push_back(snap);
        }
    }

    return result;
}
