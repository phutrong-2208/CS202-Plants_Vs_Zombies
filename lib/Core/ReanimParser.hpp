#ifndef REANIMPARSER_HPP
#define REANIMPARSER_HPP

#include <Common.hpp>

struct Frame{
    float snap;
    float newX, newY;
    float skewX, skewY;
    float scaleX, scaleY;
    float alpha; // opacity
    std :: string imageName;

    Frame();
    std::string getTextureKey() const;
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

    Frame getInterpolatedFrame(float time) const;
};
    
class ReanimParser{
private:
    float fps;  
    std :: vector<ReanimTrack> trackList;

    std :: string getTagContent(const std :: string &src, const std :: string& nameTag, size_t&pos);
    //getTagContent to find the tag with name "nameTag" <nameTag> ... <\nameTag> from src
public:
    bool loadFromFile(const std :: string& path); //parser process

    float getDuration() const;
    float getLoopStartTime() const; // Safe loop start time (ignores initial hidden frames)
    int getTrackCount() const;

    const ReanimTrack * getTrack(int index) const;
    const ReanimTrack * getTrack(const std :: string& name) const;
};

#endif