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
    if (rawAnim == nullptr || rawTexPack == nullptr) return nullptr;
    const ReanimTrack* track = rawAnim->getTrack(trackName);
    if (track == nullptr) return nullptr;
    
    const float loopStart = (clipLoopStart >= 0.0f) ? clipLoopStart : rawAnim->getLoopStartTime();
    Frame frame = track->getInterpolatedFrame(currentTime, loopStart, clipEnd);
    return rawTexPack->GetTexture(frame.getTextureKey());
}

bool ReanimInstance::playClip(const std::string& clipName) {
    if (rawAnim == nullptr) return false;

    const AnimClip* clip = rawAnim -> getClip(clipName);
    if (clip == nullptr) {
        TraceLog(LOG_WARNING, "ReanimInstance: clip '%s' not found", clipName.c_str());
        return false;
    }

    clipLoopStart = clip -> loopStart;
    clipEnd       = clip -> endTime;
    currentTime   = clip -> startTime;
    return true;
}

// -----------------------------------------------------------------------
// Playback
// -----------------------------------------------------------------------

void ReanimInstance::updateTime(float deltaSeconds) {
    if (rawAnim == nullptr) return;

    currentTime += deltaSeconds * speed;

    // If a specific clip has been selected, loop within [clipLoopStart, clipEnd].
    // Otherwise (clipLoopStart == -1) loop the entire file — this is the normal
    // case for idle animations where the whole file IS the animation cycle.
    if (clipLoopStart >= 0.0f) {
        if (looping && clipEnd > clipLoopStart && currentTime >= clipEnd) {
            float range = clipEnd - clipLoopStart;
            currentTime = clipLoopStart + std::fmod(currentTime - clipLoopStart, range);
        }
        if (!looping) {
            currentTime = std::min(currentTime, clipEnd);
        }
    } else {
        const float duration = rawAnim->getDuration();
        if (looping && currentTime >= duration) {
            currentTime = std::fmod(currentTime, duration);
        }
        if (!looping) {
            currentTime = std::min(currentTime, duration);
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

// -----------------------------------------------------------------------
// Rendering
// -----------------------------------------------------------------------

void ReanimInstance::draw(Rectangle hitbox) const {
    if (rawAnim == nullptr || rawTexPack == nullptr) {
        TraceLog(LOG_ERROR, "ReanimInstance: animation or texture package not set");
        return;
    }

    // loopStart: used as the "rest pose" time for tracks that are invisible
    // at currentTime.  When a clip is active we use its loopStart; otherwise
    // we use the file-wide heuristic (most common first-visible snap).
    const float loopStart = (clipLoopStart >= 0.0f) ? clipLoopStart : rawAnim->getLoopStartTime();
    const int   trackCount = rawAnim->getTrackCount();

    for (int i = 0; i < trackCount; ++i) {
        const ReanimTrack* track = rawAnim->getTrack(i);
        if (track == nullptr) continue;

        const std :: string trackName = track -> getTrackName();
        if(hiddenTracks.find(trackName) != hiddenTracks.end()) continue;
        if(useVisibleTrackFilter &&
           visibleTracks.find(trackName) == visibleTracks.end()) continue;

        Frame frame = track -> getInterpolatedFrame(currentTime, loopStart, clipEnd);
        if (frame.alpha <= 0.0f) continue;

        Texture2D* currentTex = rawTexPack->GetTexture(frame.getTextureKey());
        if (currentTex == nullptr) continue;

        const float kx = frame.skewX * DEG2RAD;
        const float ky = frame.skewY * DEG2RAD;

        const float a  =  frame.scaleX * cosf(kx) * scalar;
        const float b_ =  frame.scaleX * sinf(kx) * scalar;
        const float c  = -frame.scaleY * sinf(ky) * scalar;
        const float d  =  frame.scaleY * cosf(ky) * scalar;
        const float tx =  hitbox.x + frame.newX * scalar;
        const float ty =  hitbox.y + frame.newY * scalar;

        // Column-major (OpenGL / rlgl):
        // | a   c   0  tx |
        // | b_  d   0  ty |
        // | 0   0   1   0 |
        // | 0   0   0   1 |
        const float mat[16] = {
            a,    b_,   0.0f, 0.0f,   // column 0
            c,    d,    0.0f, 0.0f,   // column 1
            0.0f, 0.0f, 1.0f, 0.0f,   // column 2
            tx,   ty,   0.0f, 1.0f    // column 3 (translation)
        };

        rlPushMatrix();
        rlMultMatrixf(mat);

        const float w = (float)currentTex->width;
        const float h = (float)currentTex->height;

        const unsigned char alphaByte = (unsigned char)(frame.alpha * 255.0f);
        const Color tint = {255, 255, 255, alphaByte};

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
}
