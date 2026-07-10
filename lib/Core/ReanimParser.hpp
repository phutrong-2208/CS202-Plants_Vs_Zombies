#ifndef REANIMPARSER_HPP
#define REANIMPARSER_HPP

#include <Common.hpp>

struct Frame{
    float snap;
    float newX, newY;
    float skewX, skewY;
    float scaleX, scaleY;
    float alpha; // opacity  0.0 = hidden, 1.0 = fully visible
    std :: string imageName;

    Frame();
    std::string getTextureKey() const;
};

// -----------------------------------------------------------------
// AnimClip — represents a named sub-animation inside a .reanim file.
// A .reanim can contain multiple clips packed into one timeline.
// Each clip is described by a control track (name starts with "anim_").
//
//   startTime  — first frame of this clip (inclusive)
//   loopStart  — where the loop rewinds to (first frame visible in the clip)
//   endTime    — last frame of this clip (exclusive)
// -----------------------------------------------------------------
struct AnimClip {
    std :: string name;       // e.g. "idle", "shooting" (strips "anim_" prefix)
    float startTime  = 0.0f;
    float loopStart  = 0.0f; // rewind point when looping
    float endTime    = 0.0f;
};

class ReanimTrack{
private:
    std :: string name;
    std :: vector<Frame> transforms;   
public:
    void setTrackName(const std::string& trackName);
    std::string getTrackName() const;

    void pushFrame(const Frame& newFrame);
    const std::vector <Frame>& getFullTrack() const;
    int getFrameCount() const;
    float getDuration() const;

    Frame getInterpolatedFrame(float time, float startTime, float endTime) const;
};
    
class ReanimParser{
private:
    float fps;  
    std :: vector<ReanimTrack> trackList;
    std :: vector<AnimClip>    clipList; 

    std :: string getTagContent(const std :: string &src, const std :: string& nameTag, size_t&pos);

    // Builds clipList by scanning control tracks after loadFromFile.
    void buildClips();

public:
    bool loadFromFile(const std :: string& path); //parser process

    float getDuration() const;
    float getLoopStartTime() const; // fallback when no clip is selected
    int getTrackCount() const;

    // Clip access
    const std :: vector<AnimClip>& getClips() const { return clipList; }
    const AnimClip* getClip(const std :: string& name) const;

    const ReanimTrack * getTrack(int index) const;
    const ReanimTrack * getTrack(const std :: string& name) const;
};

#endif