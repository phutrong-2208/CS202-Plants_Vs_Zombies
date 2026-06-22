#ifndef REANIMPARSER_HPP
#define REANIMPARSER_HPP

#include "Common.hpp"


struct Frame{
    float snap;
    float newX, newY;
    float skewX, skewY;
    float scaleX, scaleY;
    float alpha; // opacity
    std :: string imageName;

    Frame(){
        snap = 0;
        newX = newY = 0;
        skewX = skewY = 0;
        scaleX = scaleY = 1;
        alpha = 1;
        imageName = "";
    }
};

struct ReanimTrack{
    std :: string name;
    std :: vector<Frame> transforms;             
};
    
class ReanimParser{
private:
    float fps;  
    std :: vector<ReanimTrack> trackList;
public:
    std :: string getTagContent(const std :: string &src, const std :: string& nameTag, size_t&pos);
    //getTagContent to find the tag with name "nameTag" <nameTag> ... <\nameTag> from src
    
    bool loadFromFile(const std :: string& path); //parser process

    float getDuration() const;
    float getLoopStartTime() const; // Safe loop start time (ignores initial hidden frames)
    int getTrackCount() const;

    const ReanimTrack * getTrack(int index) const;
    const ReanimTrack * getTrack(const std :: string& name) const;

    Frame getInterpolatedFrame(const ReanimTrack& track, float time) const;
    std :: string getTextureKey(const Frame& frame) const;
};

#endif