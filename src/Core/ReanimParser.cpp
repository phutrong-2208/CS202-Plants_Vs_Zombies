#include "Core/ReanimParser.hpp"

///////////////////////////////////
///     FRAME CONSTRUCTION      ///
///////////////////////////////////
Frame::Frame(){
    snap = 0;
    newX = newY = 0;
    skewX = skewY = 0;
    scaleX = scaleY = 1;
    alpha = 1;
    imageName = "";
}

std::string Frame::getTextureKey() const {
    if(imageName.find("IMAGE_REANIM_") == 0){
        return imageName.substr(13);
    }
    return imageName;
}

///////////////////////////////////////
///     REANIMTRACK CONSTRUCTION    ///
///////////////////////////////////////
void ReanimTrack::setTrackName(const std::string& trackName) {
    name = trackName;
}
std::string ReanimTrack::getTrackName() const {
    return name;
}

void ReanimTrack::pushFrame(const Frame& newFrame) {
    transforms.push_back(newFrame);
}
const std::vector <Frame>& ReanimTrack::getFullTrack() const {
    return transforms;
}
int ReanimTrack::getFrameCount() const {
    return (int) transforms.size();
}
float ReanimTrack::getDuration() const {
    return getFrameCount() == 0 ? 0.0f : transforms.back().snap;
}

Frame ReanimTrack :: getInterpolatedFrame(float time) const{
    const std::vector <Frame>& frames = transforms;
    if(frames.empty()) return {};
    if(time <= frames[0].snap) return frames[0];
    if(time >= frames.back().snap) return frames.back();

    for (int i = 0; i < (int)frames.size() - 1; ++i){
        if(time >= frames[i].snap and time < frames[i + 1].snap){
            float alpha = (time - frames[i].snap) / (frames[i + 1].snap - frames[i].snap);

            Frame newFrame;
            newFrame.snap   = time;
            newFrame.newX   = Lerp(frames[i].newX, frames[i + 1].newX, alpha);
            newFrame.newY   = Lerp(frames[i].newY, frames[i + 1].newY, alpha);
            newFrame.skewX  = Lerp(frames[i].skewX, frames[i + 1].skewX, alpha);
            newFrame.skewY  = Lerp(frames[i].skewY, frames[i + 1].skewY, alpha);
            newFrame.scaleX = Lerp(frames[i].scaleX, frames[i + 1].scaleX, alpha);
            newFrame.scaleY = Lerp(frames[i].scaleY, frames[i + 1].scaleY, alpha);
            
            newFrame.alpha     = frames[i].alpha;
            newFrame.imageName = frames[i].imageName;
            
            return newFrame;
        }
    }

    return frames.back();
}


////////////////////////////////////////
///     REANIMPARSER CONSTRUCTION    ///
////////////////////////////////////////
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
    std :: string fullPath = PROJECT_DIR + path;
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

        track.setTrackName(getTagContent(trackInStr, "name", trackPos));

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
            track.pushFrame(frameOfTrack);
            frameIndex++;
        }

        if (track.getFrameCount() > 0){
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
        if(t.getTrackName() == name) return &t;
    }
    return nullptr;
}

float ReanimParser :: getDuration(void) const{
    float maxDuration = 0.0f;
    for (const ReanimTrack& track : trackList) {
        maxDuration = std::max(maxDuration, track.getDuration());
    }

    return maxDuration;
}

// Find the most common snap (mode) of the first visible frame of each track.
// This is the point when the MAJORITY of tracks appear simultaneously -> safe loop start point.
// Avoids the case where a single track becomes visible earlier than the rest (e.g., anim_blink).
float ReanimParser :: getLoopStartTime(void) const {
    std :: map<float, int> snapCount;
    for (const ReanimTrack& track : trackList) {
        const std::vector <Frame>& allTransforms = track.getFullTrack();
        for (const Frame& f : allTransforms) {
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