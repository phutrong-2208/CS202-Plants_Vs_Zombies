#include "Core/AnimationManager.hpp"
#include <rlgl.h> 
AnimationManager::AnimationManager() : currentClip(-1), currentTime(0.0f), speed(1.0f), looping(true) {}

void AnimationManager::addClip(const std::string& name, ReanimParser* parser) {
    clipList.push_back({name, parser});
}

void AnimationManager::play(const std::string& name) {
    int cnt = 0;
    for (auto& a : clipList) {
        if (a.name == name) {
            currentClip = cnt;
            currentTime = a.animation -> getLoopStartTime();
        }
        cnt++;
    }
}

void AnimationManager::play(int index) {
    currentClip = index;
    if (index >= 0 && index < (int)clipList.size())
        currentTime = clipList[index].animation -> getLoopStartTime();
    else
        currentTime = 0.0f;
}

void AnimationManager::stop() {
    currentClip  = -1;
    currentTime  = 0.0f;
}

void AnimationManager :: update(float dt) {
    if (currentClip < 0) return;

    currentTime += dt * speed;
    const float dur = clipList[currentClip].animation -> getDuration();
    if (dur <= 0.0f) return;

    if (looping && currentTime >= dur)
        currentTime = fmod(currentTime, dur);   //mod for float type
    else if (!looping && currentTime > dur)
        currentTime = dur;
}

void AnimationManager :: draw(Vector2 position, TextureManager& texture, const std::string& packName, float scaleRatio) const {
    if (currentClip < 0) return;

    TexturePackage* pack = texture.getPackage(packName);
    if (!pack) {
        TraceLog(LOG_ERROR, "AnimationManager: texture package '%s' not found", packName.c_str());
        return;
    }

    const ReanimParser& parser = *clipList[currentClip].animation;
    const float loopStart = parser.getLoopStartTime();
    const float scale     = scaleRatio / 100.0f;
    const int   trackCount = parser.getTrackCount();

    for (int i = 0; i < trackCount; ++i) {
        const ReanimTrack* track = parser.getTrack(i);
        if (!track) continue;

        Frame frame = parser.getInterpolatedFrame(*track, currentTime);

        if (frame.alpha <= 0.0f) {
            const Frame restFrame = parser.getInterpolatedFrame(*track, loopStart);
            if (restFrame.alpha <= 0.0f) continue; 
            frame = restFrame;  
        }

        Texture2D* tex = pack -> GetTexture(parser.getTextureKey(frame));
        if (!tex) continue;

        const float kx = frame.skewX * DEG2RAD;
        const float ky = frame.skewY * DEG2RAD;

        const float a  =  frame.scaleX * cosf(ky) * scale;
        const float b_ =  frame.scaleX * sinf(ky) * scale; 
        const float c  = -frame.scaleY * sinf(kx) * scale;
        const float d  =  frame.scaleY * cosf(kx) * scale;
        const float tx =  position.x + frame.newX * scale;
        const float ty =  position.y + frame.newY * scale;

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

        const float w = (float)tex -> width;
        const float h = (float)tex -> height;
        DrawTexturePro(*tex, {0, 0, w, h}, {0, 0, w, h}, {0, 0}, 0.0f, WHITE);

        rlPopMatrix();
    }
}