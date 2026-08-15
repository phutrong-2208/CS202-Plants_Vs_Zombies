#include "Core/UserProfileManager.hpp"

#include <cctype>
#include <filesystem>

namespace {
    const std :: filesystem :: path PROFILE_CONFIG_PATH =
        std :: filesystem :: path(PROJECT_DIR) /
        "assets/data/config/UserProfileConfig.txt";
    const std :: filesystem :: path PROFILE_DIRECTORY =
        std :: filesystem :: path(PROJECT_DIR) / "assets/data/users";

    std :: string trim(const std :: string& value) {
        size_t first = 0;
        while(first < value.size() &&
              std :: isspace(static_cast<unsigned char>(value[first]))) {
            first++;
        }

        size_t last = value.size();
        while(last > first &&
              std :: isspace(static_cast<unsigned char>(value[last - 1]))) {
            last--;
        }
        return value.substr(first, last - first);
    }

    bool parseLevelID(const std :: string& value, LevelID& level) {
        const size_t comma = value.find(',');
        if(comma == std :: string :: npos) return false;

        try {
            const std :: string worldText = trim(value.substr(0, comma));
            const std :: string stageText = trim(value.substr(comma + 1));
            if(worldText.empty() || stageText.empty()) return false;

            level.world = std :: stoi(worldText);
            level.stage = std :: stoi(stageText);
            return level.world > 0 && level.stage > 0;
        }
        catch(const std :: exception&) {
            return false;
        }
    }

    bool findPlantType(const std :: string& name, PlantType& type) {
        for(int index = 0; index < static_cast<int>(PLANT_COUNT); index++) {
            const PlantType candidate = static_cast<PlantType>(index);
            if(name == getPlantTextureKey(candidate)) {
                type = candidate;
                return true;
            }
        }
        return false;
    }

    std :: bitset<PLANT_COUNT> parseUnlockedPlants(const std :: string& value) {
        std :: bitset<PLANT_COUNT> result;
        std :: string token;
        std :: stringstream stream(value);

        while(std :: getline(stream, token, ',')) {
            PlantType type = PLANT_COUNT;
            const std :: string plantName = trim(token);
            if(findPlantType(plantName, type)) {
                result.set(static_cast<size_t>(type));
            }
            else if(!plantName.empty()) {
                TraceLog(LOG_WARNING, "Unknown plant in user profile: %s", plantName.c_str());
            }
        }
        return result;
    }

    bool isSafeProfileFileName(const std :: string& fileName) {
        if(fileName.empty()) return false;
        const std :: filesystem :: path path(fileName);
        return path.filename() == path && path.extension() == ".txt";
    }

    std :: unique_ptr<UserProfile> loadProfileFile(const std :: string& fileName) {
        if(!isSafeProfileFileName(fileName)) return nullptr;

        const std :: filesystem :: path path = PROFILE_DIRECTORY / fileName;
        std :: ifstream file(path);
        if(!file.is_open()) {
            TraceLog(LOG_WARNING, "Cannot load user profile: %s", path.string().c_str());
            return nullptr;
        }

        std :: string profileName = "Anonymous";
        LevelID highestReached{1, 1};
        std :: bitset<PLANT_COUNT> unlockedPlants;
        unlockedPlants.set(static_cast<size_t>(PEASHOOTER));
        int highestScore = 0;
        std :: string line;

        while(std :: getline(file, line)) {
            line = trim(line);
            if(line.empty() || line[0] == '#') continue;

            const size_t equals = line.find('=');
            if(equals == std :: string :: npos) continue;

            const std :: string key = trim(line.substr(0, equals));
            const std :: string value = trim(line.substr(equals + 1));
            if(key == "NAME") {
                profileName = value;
            }
            else if(key == "HIGHEST_REACHED") {
                LevelID parsedLevel{1, 1};
                if(parseLevelID(value, parsedLevel)) highestReached = parsedLevel;
            }
            else if(key == "UNLOCKED_PLANTS") {
                unlockedPlants = parseUnlockedPlants(value);
            }
            else if(key == "HIGHEST_SCORE") {
                try { highestScore = std::max(0, std::stoi(value)); }
                catch(const std :: exception&) {}
            }
        }

        auto profile = std :: make_unique<UserProfile>(profileName);
        profile -> setHighestUnlockedLevel(highestReached);
        profile -> setUnlockedPlants(unlockedPlants);
        profile -> setHighestSurvivalScore(highestScore);
        return profile;
    }

    bool saveProfileFile(const UserProfile& profile, const std :: string& fileName) {
        if(!isSafeProfileFileName(fileName)) return false;

        const std :: filesystem :: path path = PROFILE_DIRECTORY / fileName;
        std :: ofstream file(path, std :: ios :: trunc);
        if(!file.is_open()) {
            TraceLog(LOG_ERROR, "Cannot save user profile: %s", path.string().c_str());
            return false;
        }

        const LevelID& level = profile.getHighestUnlockedLevel();
        file << "# User profile data.\n\n";
        file << "NAME=" << profile.getProfileName() << "\n";
        file << "HIGHEST_REACHED=" << level.world << ',' << level.stage << "\n";
        file << "HIGHEST_SCORE=" << profile.getHighestSurvivalScore() << "\n";
        file << "UNLOCKED_PLANTS=";

        bool firstPlant = true;
        const auto& unlockedPlants = profile.getUnlockedPlants();
        for(int index = 0; index < static_cast<int>(PLANT_COUNT); index++) {
            if(!unlockedPlants.test(static_cast<size_t>(index))) continue;

            if(!firstPlant) file << ',';
            file << getPlantTextureKey(static_cast<PlantType>(index));
            firstPlant = false;
        }
        file << "\n";
        return file.good();
    }
}

UserProfileManager :: UserProfileManager() {
    if(loadProfiles()) return;

    profiles.push_back(std :: make_unique<UserProfile>("Anonymous"));
    profileFiles.push_back("Anonymous.txt");
    activeProfileIndex = 0;
    saveProfiles();
}

bool UserProfileManager :: isValidIndex(int index) const {
    return index >= 0 and index < static_cast<int>(profiles.size());
}

std :: string UserProfileManager :: createProfileFileName() const {
    for(int id = 1; ; id++) {
        const std :: string candidate = "Profile_" + std :: to_string(id) + ".txt";
        const bool alreadyUsed = std :: find(
            profileFiles.begin(), profileFiles.end(), candidate
        ) != profileFiles.end();

        if(!alreadyUsed && !std :: filesystem :: exists(PROFILE_DIRECTORY / candidate)) {
            return candidate;
        }
    }
}

bool UserProfileManager :: loadProfiles() {
    std :: ifstream file(PROFILE_CONFIG_PATH);
    if(!file.is_open()) {
        TraceLog(
            LOG_WARNING,
            "Cannot load user profile index: %s",
            PROFILE_CONFIG_PATH.string().c_str()
        );
        return false;
    }

    std :: vector<std :: unique_ptr<UserProfile>> loadedProfiles;
    std :: vector<std :: string> loadedProfileFiles;
    int loadedActiveIndex = 0;
    std :: string line;

    while(std :: getline(file, line)) {
        line = trim(line);
        if(line.empty() || line[0] == '#') continue;

        const size_t equals = line.find('=');
        if(equals == std :: string :: npos) continue;

        const std :: string key = trim(line.substr(0, equals));
        const std :: string value = trim(line.substr(equals + 1));

        if(key == "ACTIVE_PROFILE_INDEX") {
            try {
                loadedActiveIndex = std :: stoi(value);
            }
            catch(const std :: exception&) {
                loadedActiveIndex = 0;
            }
        }
        else if(key == "PROFILE_FILE" && isSafeProfileFileName(value)) {
            std :: unique_ptr<UserProfile> profile = loadProfileFile(value);
            if(profile) {
                loadedProfiles.push_back(std :: move(profile));
                loadedProfileFiles.push_back(value);
            }
        }
    }

    if(loadedProfiles.empty()) {
        TraceLog(
            LOG_WARNING,
            "No valid user profile found in: %s",
            PROFILE_CONFIG_PATH.string().c_str()
        );
        return false;
    }

    profiles = std :: move(loadedProfiles);
    profileFiles = std :: move(loadedProfileFiles);
    activeProfileIndex = std :: clamp(
        loadedActiveIndex,
        0,
        static_cast<int>(profiles.size()) - 1
    );
    return true;
}

bool UserProfileManager :: saveProfiles() const {
    std :: error_code directoryError;
    std :: filesystem :: create_directories(PROFILE_DIRECTORY, directoryError);
    if(directoryError) {
        TraceLog(
            LOG_ERROR,
            "Cannot create user profile directory: %s",
            PROFILE_DIRECTORY.string().c_str()
        );
        return false;
    }

    if(profiles.size() != profileFiles.size()) return false;
    for(size_t index = 0; index < profiles.size(); index++) {
        if(!profiles[index] ||
           !saveProfileFile(*profiles[index], profileFiles[index])) {
            return false;
        }
    }

    std :: ofstream file(PROFILE_CONFIG_PATH, std :: ios :: trunc);
    if(!file.is_open()) {
        TraceLog(
            LOG_ERROR,
            "Cannot save user profile index: %s",
            PROFILE_CONFIG_PATH.string().c_str()
        );
        return false;
    }

    file << "# User profile index. Individual profiles are stored in assets/data/users.\n\n";
    file << "ACTIVE_PROFILE_INDEX=" << activeProfileIndex << "\n";
    for(const std :: string& fileName : profileFiles) {
        file << "PROFILE_FILE=" << fileName << "\n";
    }

    if(!file.good()) {
        TraceLog(
            LOG_ERROR,
            "Failed while saving user profile index: %s",
            PROFILE_CONFIG_PATH.string().c_str()
        );
        return false;
    }
    return true;
}

void UserProfileManager :: loadProfile(int index) {
    selectProfile(index);
}

void UserProfileManager :: saveProfile(int index) {
    if(!isValidIndex(index)) return;
    saveProfiles();
}

void UserProfileManager :: addProfile(std :: unique_ptr<UserProfile> profile) {
    if(!profile) return;

    profileFiles.push_back(createProfileFileName());
    profiles.push_back(std :: move(profile));
    if(activeProfileIndex < 0) activeProfileIndex = 0;
    saveProfiles();
}

void UserProfileManager :: delProfile(int index) {
    if(!isValidIndex(index) || profiles.size() <= 1) return;

    const std :: filesystem :: path deletedProfilePath =
        PROFILE_DIRECTORY / profileFiles[index];
    profiles.erase(profiles.begin() + index);
    profileFiles.erase(profileFiles.begin() + index);

    if(activeProfileIndex == index) {
        activeProfileIndex = std :: min(index, static_cast<int>(profiles.size()) - 1);
    }
    else if(activeProfileIndex > index) {
        activeProfileIndex--;
    }
    if(saveProfiles()) {
        std :: error_code removeError;
        std :: filesystem :: remove(deletedProfilePath, removeError);
        if(removeError) {
            TraceLog(
                LOG_WARNING,
                "Cannot delete user profile file: %s",
                deletedProfilePath.string().c_str()
            );
        }
    }
}

int UserProfileManager :: getProfileCount() const {
    return (int)(profiles.size());
}

int UserProfileManager :: getActiveProfileIndex() const {
    return activeProfileIndex;
}

UserProfile* UserProfileManager :: getProfile(int index) {
    return isValidIndex(index) ? profiles[index].get() : nullptr;
}

const UserProfile* UserProfileManager :: getProfile(int index) const {
    return isValidIndex(index) ? profiles[index].get() : nullptr;
}

UserProfile* UserProfileManager :: getActiveProfile() {
    return getProfile(activeProfileIndex);
}

const UserProfile* UserProfileManager :: getActiveProfile() const {
    return getProfile(activeProfileIndex);
}

bool UserProfileManager :: selectProfile(int index) {
    if(!isValidIndex(index)) return false;
    activeProfileIndex = index;
    saveProfiles();
    return true;
}

bool UserProfileManager :: renameProfile(int index, const std :: string& name) {
    UserProfile* profile = getProfile(index);
    if(!profile || name.empty()) return false;

    profile -> setProfileName(name);
    saveProfiles();
    return true;
}
