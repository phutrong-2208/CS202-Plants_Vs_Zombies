#include "Gameplay/Levels/Level.hpp"

namespace {
    std :: string trim(const std :: string& value) {
        const size_t begin = value.find_first_not_of(" \t\r\n");
        if(begin == std :: string :: npos) return "";

        const size_t end = value.find_last_not_of(" \t\r\n");
        return value.substr(begin, end - begin + 1);
    }
}

Level :: Level(LevelID _id) : id(_id) {
    loadFromFile();
}

void Level :: loadFromFile() {
    const std :: string path =
        std :: string(PROJECT_DIR) + "assets/data/levels/" + id.getFileName();

    std :: ifstream file(path);
    if(!file.is_open()) {
        TraceLog(LOG_ERROR, "Level: cannot open %s", path.c_str());
        return;
    }

    bool readingLevelSection = false;
    std :: string line;

    while(std :: getline(file, line)) {
        line = trim(line);
        if(line.empty() || line.front() == '#') continue;

        if(line.front() == '[' && line.back() == ']') {
            readingLevelSection = line == "[LEVEL]";
            continue;
        }

        if(!readingLevelSection) continue;

        const size_t equalPosition = line.find('=');
        if(equalPosition == std :: string :: npos) continue;

        const std :: string key = trim(line.substr(0, equalPosition));
        const std :: string value = trim(line.substr(equalPosition + 1));

        if(key == "LANES") {
            parseLanes(value);
        }
        else if(key == "USE_SOD_ROLL_INTRO") {
            useSodRollIntro = value == "true" || value == "TRUE" || value == "1";
        }
    }
}

void Level :: parseLanes(const std :: string& value) {
    lanes.clear();

    std :: string laneName;
    std :: stringstream stream(value);
    while(std :: getline(stream, laneName, ',')) {
        laneName = trim(laneName);

        if(laneName == "GRASS") lanes.push_back(LaneType :: GRASS);
        else if(laneName == "POOL") lanes.push_back(LaneType :: POOL);
        else if(laneName == "ROOF") lanes.push_back(LaneType :: ROOF);
        else if(laneName == "INACTIVE") lanes.push_back(LaneType :: INACTIVE);
        else TraceLog(LOG_WARNING, "Level: unknown lane type '%s'", laneName.c_str());
    }
}

const WaveList& Level :: getWave() const{
    return waves;
}

const std :: vector<LaneType>& Level :: getLanes() const {
    return lanes;
}

bool Level :: usesSodRollIntro() const {
    return useSodRollIntro;
}
