#ifndef USER_PROFILE_MANAGER_HPP
#define USER_PROFILE_MANAGER_HPP

#include "Common.hpp"
#include "Core/UserProfile.hpp"

class UserProfileManager {
private:
    std :: vector<std :: unique_ptr<UserProfile>> profiles;
    std :: vector<std :: string> profileFiles;
    int activeProfileIndex = -1;

    bool isValidIndex(int index) const;
    std :: string createProfileFileName() const;

public:
    UserProfileManager();

    bool loadProfiles();
    bool saveProfiles() const;

    void loadProfile(int index);
    void saveProfile(int index);
    void addProfile(std :: unique_ptr<UserProfile> profile);
    void delProfile(int index);

    int getProfileCount() const;
    int getActiveProfileIndex() const;
    UserProfile* getProfile(int index);
    const UserProfile* getProfile(int index) const;
    UserProfile* getActiveProfile();
    const UserProfile* getActiveProfile() const;

    bool selectProfile(int index);
    bool renameProfile(int index, const std :: string& name);
};

#endif
