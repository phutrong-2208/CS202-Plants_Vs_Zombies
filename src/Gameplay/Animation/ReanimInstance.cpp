#include <Gameplay/Animation/ReanimInstance.hpp>
#include <rlgl.h>

void ReanimInstance::setAnimation(ReanimParser* Anim) {
    rawAnim = Anim;
}
void ReanimInstance::setTexturePackage(TexturePackage* TexPack) {
    rawTexPack = TexPack;
}

void ReanimInstance::updateTime(float deltaSeconds) {
    if (rawAnim == nullptr) return;
    currentTime += deltaSeconds * speed;

    const float duration = rawAnim -> getDuration();
    if (looping && currentTime >= duration) {
        currentTime = fmod(currentTime, duration);
    }
    if (!looping) {
        currentTime = std::min(currentTime, duration);
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

void ReanimInstance::draw(Rectangle hitbox) {
    if (rawAnim == nullptr || rawTexPack == nullptr) {
        TraceLog(LOG_ERROR, "AnimationManager: Animation package not found");
        return;
    }

    // const ReanimParser& parser = *clipList[currentClip].animation;
    // const float loopStart = parser.getLoopStartTime();
    // const float scale     = scaleRatio / 100.0f;

    const float scale = 1.0f;
    const float loopStart = rawAnim -> getLoopStartTime();
    const int   trackCount = rawAnim -> getTrackCount();

    for (int i = 0; i < trackCount; ++i) {
        const ReanimTrack* track = rawAnim -> getTrack(i);
        if (track == nullptr) continue;

        Frame frame = track -> getInterpolatedFrame(currentTime);
        
        if (frame.alpha <= 0.0f) {
            const Frame restFrame = track -> getInterpolatedFrame(loopStart);

            if (restFrame.alpha <= 0.0f) continue; 
            frame = restFrame;  
        }

        Texture2D* currentTex = rawTexPack -> GetTexture(frame.getTextureKey());
        if (currentTex == nullptr) continue;

        const float kx = frame.skewX * DEG2RAD;
        const float ky = frame.skewY * DEG2RAD;

        const float a  =  frame.scaleX * cosf(ky) * scale;
        const float b_ =  frame.scaleX * sinf(ky) * scale; 
        const float c  = -frame.scaleY * sinf(kx) * scale;
        const float d  =  frame.scaleY * cosf(kx) * scale;
        const float tx =  hitbox.x + frame.newX * scale;
        const float ty =  hitbox.y + frame.newY * scale;

        // Column-major (OpenGL / rlgl):
        // | a   c   0  tx |
        // | b_  d   0  ty |
        // | 0   0   1   0 |
        // | 0   0   0   1 |
        const float mat[16] = {
            a,    b_,   0.0f, 0.0f,   // column 1
            c,    d,    0.0f, 0.0f,   // column 2
            0.0f, 0.0f, 1.0f, 0.0f,   // column 3
            tx,   ty,   0.0f, 1.0f    // column 4
        };

        rlPushMatrix();
        rlMultMatrixf(mat);

        const float w = (float) currentTex -> width;
        const float h = (float) currentTex -> height;
        DrawTexturePro(*currentTex, {0, 0, w, h}, {0, 0, w, h}, {0, 0}, 0.0f, WHITE);

        rlPopMatrix();
    }
}