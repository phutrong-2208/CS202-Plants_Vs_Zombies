#ifndef SOUND_MANAGER_HPP
#define SOUND_MANAGER_HPP

#include <Common.hpp>

class SoundManager {
private:
    struct SoundAsset {
        Sound sound{};
        float volume = 1.0f;
    };

    std :: map<std :: string, SoundAsset> sounds;
    float masterVolume = 1.0f;
    bool muted = false;

    std :: string normalizeKey(const std :: string& key) const;
    void applyVolume(SoundAsset& asset, float volumeScale = 1.0f) const;

public:
    SoundManager() = default;
    ~SoundManager();

    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    bool addSound(const std :: string& key, const std :: string& path);
    bool removeSound(const std :: string& key);
    bool hasSound(const std :: string& key) const;

    bool play(const std :: string& key);
    bool play(
        const std :: string& key,
        float volumeScale,
        float pitch = 1.0f,
        float pan = 0.5f
    );
    void stop(const std :: string& key);
    void pause(const std :: string& key);
    void resume(const std :: string& key);
    void stopAll();
    bool isPlaying(const std :: string& key) const;

    void setSoundVolume(const std :: string& key, float volume);
    void setMasterVolume(float volume);
    float getMasterVolume() const;
    void setMuted(bool shouldMute);
    bool isMuted() const;
    void clear();
};

#endif
