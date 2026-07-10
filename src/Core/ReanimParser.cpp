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
const std::vector<Frame>& ReanimTrack::getFullTrack() const {
    return transforms;
}
int ReanimTrack::getFrameCount() const {
    return (int) transforms.size();
}
float ReanimTrack::getDuration() const {
    return getFrameCount() == 0 ? 0.0f : transforms.back().snap;
}

Frame ReanimTrack::getInterpolatedFrame(float time) const {
    const std::vector<Frame>& frames = transforms;
    if (frames.empty()) return {};

    // Helper: find the nearest non-empty imageName at or before index idx.
    // In Reanim, a missing <i> tag means "keep the previous frame's image".
    auto inheritImage = [&](Frame f, int idx) -> Frame {
        if (!f.imageName.empty()) return f;
        for (int k = idx; k >= 0; --k) {
            if (!frames[k].imageName.empty()) {
                f.imageName = frames[k].imageName;
                return f;
            }
        }
        return f; // still empty; caller will skip via nullptr texture lookup
    };

    if (time <= frames[0].snap) return inheritImage(frames[0], 0);
    if (time >= frames.back().snap) return inheritImage(frames.back(), (int)frames.size() - 1);

    for (int i = 0; i < (int)frames.size() - 1; ++i) {
        if (time >= frames[i].snap && time < frames[i + 1].snap) {
            float t = (time - frames[i].snap) / (frames[i + 1].snap - frames[i].snap);

            Frame newFrame;
            newFrame.snap   = time;
            newFrame.newX   = Lerp(frames[i].newX,   frames[i + 1].newX,   t);
            newFrame.newY   = Lerp(frames[i].newY,   frames[i + 1].newY,   t);
            newFrame.skewX  = Lerp(frames[i].skewX,  frames[i + 1].skewX,  t);
            newFrame.skewY  = Lerp(frames[i].skewY,  frames[i + 1].skewY,  t);
            newFrame.scaleX = Lerp(frames[i].scaleX, frames[i + 1].scaleX, t);
            newFrame.scaleY = Lerp(frames[i].scaleY, frames[i + 1].scaleY, t);
            newFrame.alpha = frames[i].alpha;

            newFrame.imageName = frames[i].imageName;

            return inheritImage(newFrame, i);
        }
    }

    return inheritImage(frames.back(), (int)frames.size() - 1);
}


////////////////////////////////////////
///     REANIMPARSER CONSTRUCTION    ///
////////////////////////////////////////
std::string ReanimParser::getTagContent(const std::string& src, const std::string& nameTag, size_t& pos) {
    std::string open = "<" + nameTag;
    size_t start = src.find(open, pos);

    if (start == std::string::npos)
        return "";

    size_t endOfOpenTag = src.find(">", start);

    if (endOfOpenTag == std::string::npos)
        return "";

    std::string close = "</" + nameTag + ">";

    start = endOfOpenTag + 1;

    size_t end = src.find(close, start);

    if (end == std::string::npos)
        return "";

    pos = end + close.size();
    return src.substr(start, end - start);
}

bool ReanimParser::loadFromFile(const std::string& path) {
    std::string fullPath = PROJECT_DIR + path;
    std::ifstream f(fullPath);

    if (!f.is_open()) {
        f.open(path);
        if (!f.is_open()) {
            TraceLog(LOG_ERROR, "REANIM: Cannot Open The File %s", path.c_str());
            return false;
        }
    }

    std::stringstream token;
    token << f.rdbuf();

    std::string src = token.str();
    size_t p = 0;
    std::string FPS = getTagContent(src, "fps", p);

    fps = FPS.empty() ? 30.0f : std::stof(FPS);

    while (true) {
        std::string trackInStr = getTagContent(src, "track", p);

        if (trackInStr.empty())
            break;

        ReanimTrack track;
        size_t trackPos = 0;

        track.setTrackName(getTagContent(trackInStr, "name", trackPos));

        Frame frameOfTrack{};
        size_t frameIndex = 0;

        while (true) {
            size_t oldPos = trackPos;
            std::string tf = getTagContent(trackInStr, "t", trackPos);

            if (trackPos == oldPos)
                break;

            auto getFloatValue = [&](const std::string& tagName, float& target) {
                size_t tempPos = 0;
                std::string value = getTagContent(tf, tagName, tempPos);
                if (!value.empty()) {
                    try { target = std::stof(value); }
                    catch (...) {}
                }
            };

            getFloatValue("x",  frameOfTrack.newX);
            getFloatValue("y",  frameOfTrack.newY);
            getFloatValue("sx", frameOfTrack.scaleX);
            getFloatValue("sy", frameOfTrack.scaleY);
            getFloatValue("kx", frameOfTrack.skewX);
            getFloatValue("ky", frameOfTrack.skewY);

            size_t tempPos = 0;
            std::string fTag = getTagContent(tf, "f", tempPos);

            if (!fTag.empty()) {
                try {
                    int value = std::stoi(fTag);
                    if (value == -1) {
                        frameOfTrack.alpha = 0.0f;
                    } else {
                        frameOfTrack.alpha = (value == 0) ? 1.0f : std :: min(1.0f, (float)value / 255.0f);
                    }
                }
                catch (...) {}
            }

            tempPos = 0;
            std::string iTag = getTagContent(tf, "i", tempPos);
            if (!iTag.empty()) {
                frameOfTrack.imageName = iTag;
            }

            frameOfTrack.snap = static_cast<float>(frameIndex) / fps;
            track.pushFrame(frameOfTrack);
            frameIndex++;
        }

        if (track.getFrameCount() > 0) {
            trackList.push_back(track);
        }
    }
    // After parsing all tracks, extract named clips from control tracks.
    buildClips();

    return !trackList.empty();
}

// buildClips — reads tracks whose name starts with "anim_" and uses
// their f = -1/f = 0 pattern to define named clip ranges.
//
//   first frame with alpha > 0  -> clipStart / loopStart
//   first frame with alpha <= 0 after that → clipEnd
void ReanimParser :: buildClips() {
    clipList.clear();
    const std :: string PREFIX = "anim_";
    for (const ReanimTrack& track : trackList) {
        const std :: string& tname = track.getTrackName();
        // Only process control tracks (name starts with "anim_")
        if (tname.rfind(PREFIX, 0) != 0) continue;

        const std::vector<Frame>& frames = track.getFullTrack();
        if (frames.empty()) continue;

        std::string clipName = tname.substr(PREFIX.size());

        bool  inVisible = false;
        float visStart  = 0.0f;

        for (int i = 0; i < (int)frames.size(); ++i) {
            const Frame& fr = frames[i];

            if (!inVisible && fr.alpha > 0.0f) {
                inVisible = true;
                visStart  = fr.snap;
            }
            else if (inVisible && fr.alpha <= 0.0f) {
                inVisible = false;
                AnimClip clip;
                clip.name      = clipName;
                clip.startTime = visStart;
                clip.loopStart = visStart;
                clip.endTime   = fr.snap;
                clipList.push_back(clip);
            }
        }

        // Clip that runs to the end of track without an explicit f = -1
        if (inVisible) {
            AnimClip clip;
            clip.name      = clipName;
            clip.startTime = visStart;
            clip.loopStart = visStart;
            clip.endTime   = frames.back().snap;
            clipList.push_back(clip);
        }
    }
}

const AnimClip* ReanimParser :: getClip(const std :: string& name) const {
    for (const AnimClip& c : clipList) {
        if (c.name == name) return &c;
    }
    return nullptr;
}

const ReanimTrack* ReanimParser :: getTrack(int index) const {
    if (index < 0 || index >= (int)trackList.size()) return nullptr;
    return &trackList[index];
}

const ReanimTrack* ReanimParser :: getTrack(const std::string& name) const {
    for (const auto& t : trackList) {
        if (t.getTrackName() == name) return &t;
    }
    return nullptr;
}

float ReanimParser :: getDuration() const {
    float maxDuration = 0.0f;
    for (const ReanimTrack& track : trackList) {
        maxDuration = std::max(maxDuration, track.getDuration());
    }
    return maxDuration;
}

// Fallback loopStart used when no specific clip is selected.
// Finds the snap where the majority of visual tracks first become visible.
float ReanimParser :: getLoopStartTime() const {
    std::map<float, int> snapCount;
    for (const ReanimTrack& track : trackList) {
        const std::vector<Frame>& allTransforms = track.getFullTrack();
        for (const Frame& f : allTransforms) {
            if (f.alpha > 0.0f) {
                snapCount[f.snap]++;
                break; // only the first visible frame of each track
            }
        }
    }
    if (snapCount.empty()) return 0.0f;

    float bestSnap  = 0.0f;
    int   bestCount = 0;
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