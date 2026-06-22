#include "Core/ReanimParser.hpp"

std::string ReanimParser :: getTagContent(const std :: string& src, const std :: string& nameTag, size_t& pos){
    std :: string open = "<" + nameTag;
    size_t start = src.find(open, pos);

    if (start == std :: string :: npos)
        return "";

    size_t endOfOpenTag = src.find(">", start);

    if (endOfOpenTag == std :: string :: npos)
        return "";

    std :: string close = "</" + nameTag + ">";

    start = endOfOpenTag + 1;

    size_t end = src.find(close, start);

    if (end == std :: string :: npos)
        return "";

    pos = end + close.size();
    return src.substr(start, end - start);
}

bool ReanimParser::loadFromFile(const std::string& path){
    std :: string base = GetApplicationDirectory();
    std :: string fullPath = base + path;
    std :: ifstream f(fullPath);

    if (!f.is_open()){
        f.open(path);

        if (!f.is_open()){
            TraceLog(LOG_ERROR, "REANIM: Cannot Open The File %s", path.c_str());
            return false;
        }
    }

    std :: stringstream token;
    token << f.rdbuf();

    std :: string src = token.str();
    size_t p = 0;
    std :: string FPS = getTagContent(src, "fps", p);

    fps = FPS.empty() ? 30.0f : std::stof(FPS);

    while (true){
        std :: string trackInStr = getTagContent(src, "track", p);

        if (trackInStr.empty())
            break;

        ReanimTrack track;

        size_t trackPos = 0;

        track.name = getTagContent(trackInStr, "name", trackPos);

        Frame frameOfTrack{};
        size_t frameIndex = 0;

        while (true){
            size_t oldPos = trackPos;
            std :: string tf = getTagContent(trackInStr, "t", trackPos);

            if (trackPos == oldPos)
                break;

            auto getFloatValue = [&](const std :: string& tagName, float& target){
                size_t tempPos = 0;

                std :: string value = getTagContent(tf, tagName, tempPos);

                if (!value.empty()){
                    try{
                        target = std::stof(value);
                    }
                    catch (...){}
                }
            };

            getFloatValue("x", frameOfTrack.newX);
            getFloatValue("y", frameOfTrack.newY);
            getFloatValue("sx", frameOfTrack.scaleX);
            getFloatValue("sy", frameOfTrack.scaleY);
            getFloatValue("kx", frameOfTrack.skewX);
            getFloatValue("ky", frameOfTrack.skewY);

            size_t tempPos = 0;

            std :: string fTag = getTagContent(tf, "f", tempPos);

            if (!fTag.empty()){
                try{
                    int value = std :: stoi(fTag);
                    frameOfTrack.alpha = (value == -1) ? 0.0f : 1.0f;
                }
                catch (...){}
            }

            tempPos = 0;

            std :: string iTag = getTagContent(tf, "i", tempPos);

            if (!iTag.empty()){
                frameOfTrack.imageName = iTag;
            }

            frameOfTrack.snap = static_cast<float>(frameIndex) / fps;
            track.transforms.push_back(frameOfTrack);
            frameIndex++;
        }

        if (!track.transforms.empty()){
            trackList.push_back(track);
        }
    }
    return !trackList.empty();
}

const ReanimTrack* ReanimParser :: getTrack (int index) const {
    if(index < 0 || index >= trackList.size()) return nullptr;
    return &trackList[index];
} 

const ReanimTrack* ReanimParser :: getTrack(const std :: string& name) const {
    for (auto& t : trackList){
        if(t.name == name) return &t;
    }
    return nullptr;
}

Frame ReanimParser :: getInterpolatedFrame(const ReanimTrack& track, float time) const{
    const auto& frames = track.transforms;
    if(frames.empty()) return {};
    if(time <= frames[0].snap) return frames[0];
    if(time >= frames.back().snap) return frames.back();

    for (int i = 0; i < (int)frames.size() - 1; ++i){
        if(time >= frames[i].snap and time < frames[i + 1].snap){
            float t = (time - frames[i].snap) / (frames[i + 1].snap - frames[i].snap);

            Frame newFrame;
            newFrame.snap   = time;
            newFrame.newX   = frames[i].newX   + (frames[i + 1].newX   - frames[i].newX)   * t;
            newFrame.newY   = frames[i].newY   + (frames[i + 1].newY   - frames[i].newY)   * t;
            newFrame.skewX  = frames[i].skewX  + (frames[i + 1].skewX  - frames[i].skewX)  * t;
            newFrame.skewY  = frames[i].skewY  + (frames[i + 1].skewY  - frames[i].skewY)  * t;
            newFrame.scaleX = frames[i].scaleX + (frames[i + 1].scaleX - frames[i].scaleX) * t;
            newFrame.scaleY = frames[i].scaleY + (frames[i + 1].scaleY - frames[i].scaleY) * t;
            
            newFrame.alpha     = frames[i].alpha;
            newFrame.imageName = frames[i].imageName;
            
            return newFrame;
        }
    }

    return frames.back();
}

std :: string ReanimParser :: getTextureKey(const Frame& frame) const{
    if(frame.imageName.find("IMAGE_REANIM_") == 0){
        return frame.imageName.substr(13);
    }
    return frame.imageName;
}   

float ReanimParser :: getDuration(void) const{
    float dur = 0;
    for (auto& t : trackList){
        if(!t.transforms.empty() and t.transforms.back().snap > dur){
            dur = t.transforms.back().snap;
        }
    }
    return dur;
}

// Find the most common snap (mode) of the first visible frame of each track.
// This is the point when the MAJORITY of tracks appear simultaneously -> safe loop start point.
// Avoids the case where a single track becomes visible earlier than the rest (e.g., anim_blink).
float ReanimParser :: getLoopStartTime(void) const {
    std :: map<float, int> snapCount;
    for (auto& t : trackList) {
        for (auto& f : t.transforms) {
            if (f.alpha > 0.0f) {
                snapCount[f.snap]++;
                break; // only take the first visible frame of each track
            }
        }
    }
    if (snapCount.empty()) return 0.0f;

    // Find the snap with the highest number of first visible tracks
    float bestSnap = 0.0f;
    int bestCount = 0;
    for (auto& [snap, count] : snapCount) {
        if (count > bestCount) {
            bestCount = count;
            bestSnap  = snap;
        }
    }
    return bestSnap;
}


int ReanimParser :: getTrackCount(void) const {
    return (int)trackList.size();
}

