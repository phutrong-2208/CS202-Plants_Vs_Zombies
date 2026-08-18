#ifndef MUSIC_MANAGER_HPP
#define MUSIC_MANAGER_HPP

#include <Common.hpp>

class MusicManager {
private:
    struct MusicAsset {
        Music music{};
        float volume = 1.0f;
    };

    std :: map<std :: string, MusicAsset> tracks;
    std :: string currentTrack;
    float masterVolume = 0.30f;
    bool muted = false;
    bool paused = false;

    std :: string normalizeKey(const std :: string& key) const;
    void applyVolume(MusicAsset& asset) const;

public:
    MusicManager() = default;
    ~MusicManager();

    MusicManager(const MusicManager&) = delete;
    MusicManager& operator = (const MusicManager&) = delete;

    bool addMusic(const std :: string& key, const std :: string& path);
    bool removeMusic(const std :: string& key);
    bool hasMusic(const std :: string& key) const;

    bool play(const std :: string& key, bool loop = true, bool restart = false);
    void update();
    void stop();
    void pause();
    void resume();
    void seek(float seconds);
    bool isPlaying() const;
    bool isPaused() const;
    std :: string getCurrentTrack() const;
    float getTimePlayed() const;
    float getTimeLength() const;

    void setMusicVolume(const std :: string& key, float volume);
    void setMasterVolume(float volume);
    float getMasterVolume() const;
    void setMuted(bool shouldMute);
    bool isMuted() const;
    void clear();
};

#endif
