#ifndef REANIMPARSER_HPP
#define REANIMPARSER_HPP

#include "Common.hpp"


struct Frame{
    float snap;
    float newX, newY;
    float skewX, skewY;
    float scaleX, scaleY;
    float alpha; // opacity
    int imageFrame;
};

struct ReanimTrack{
    std :: string name;
    std :: vector<Frame> transforms;             
};
    
class ReanimParser{
    float fps;  

    std :: vector<ReanimTrack> tracks;

};

#endif