#include <Core/SoundManager.hpp>
#include <cctype>

SoundManager :: ~SoundManager() {
    clear();
}

std :: string SoundManager :: normalizeKey(const std :: string& key) const {
    std :: string normalized = key;
    for(char& character : normalized) {
        character = static_cast<char>(std :: toupper(
            static_cast<unsigned char>(character)
        ));
    }
    return normalized;
}

void SoundManager :: applyVolume(SoundAsset& asset, float volumeScale) const {
    const float outputVolume = muted
        ? 0.0f
        : asset.volume * masterVolume * std :: max(0.0f, volumeScale);
    SetSoundVolume(asset.sound, std :: clamp(outputVolume, 0.0f, 1.0f));
}

bool SoundManager :: addSound(const std :: string& key, const std :: string& path) {
    if(!IsAudioDeviceReady()) {
        TraceLog(LOG_WARNING, "SoundManager: audio device is not ready");
        return false;
    }

    Sound loadedSound = LoadSound(path.c_str());
    if(!IsSoundValid(loadedSound)) {
        TraceLog(LOG_WARNING, "SoundManager: cannot load '%s'", path.c_str());
        return false;
    }

    const std :: string normalized = normalizeKey(key);
    removeSound(normalized);
    sounds[normalized] = {loadedSound, 1.0f};
    applyVolume(sounds[normalized]);
    return true;
}

bool SoundManager :: removeSound(const std :: string& key) {
    auto it = sounds.find(normalizeKey(key));
    if(it == sounds.end()) return false;

    if(IsAudioDeviceReady() && IsSoundValid(it -> second.sound)) {
        StopSound(it -> second.sound);
        UnloadSound(it -> second.sound);
    }
    sounds.erase(it);
    return true;
}

bool SoundManager :: hasSound(const std :: string& key) const {
    return sounds.find(normalizeKey(key)) != sounds.end();
}

bool SoundManager :: play(const std :: string& key) {
    return play(key, 1.0f);
}

bool SoundManager :: play(
    const std :: string& key,
    float volumeScale,
    float pitch,
    float pan
) {
    auto it = sounds.find(normalizeKey(key));
    if(it == sounds.end() || !IsAudioDeviceReady()) return false;

    applyVolume(it -> second, volumeScale);
    SetSoundPitch(it -> second.sound, std :: max(0.01f, pitch));
    SetSoundPan(it -> second.sound, std :: clamp(pan, 0.0f, 1.0f));
    PlaySound(it -> second.sound);
    return true;
}

void SoundManager :: stop(const std :: string& key) {
    auto it = sounds.find(normalizeKey(key));
    if(it != sounds.end() && IsAudioDeviceReady()) StopSound(it -> second.sound);
}

void SoundManager :: pause(const std :: string& key) {
    auto it = sounds.find(normalizeKey(key));
    if(it != sounds.end() && IsAudioDeviceReady()) PauseSound(it -> second.sound);
}

void SoundManager :: resume(const std :: string& key) {
    auto it = sounds.find(normalizeKey(key));
    if(it != sounds.end() && IsAudioDeviceReady()) ResumeSound(it -> second.sound);
}

void SoundManager :: stopAll() {
    if(!IsAudioDeviceReady()) return;
    for(auto& [key, asset] : sounds) StopSound(asset.sound);
}

bool SoundManager :: isPlaying(const std :: string& key) const {
    auto it = sounds.find(normalizeKey(key));
    return it != sounds.end() && IsAudioDeviceReady() &&
        IsSoundPlaying(it -> second.sound);
}

void SoundManager :: setSoundVolume(const std :: string& key, float volume) {
    auto it = sounds.find(normalizeKey(key));
    if(it == sounds.end()) return;

    it -> second.volume = std :: clamp(volume, 0.0f, 1.0f);
    if(IsAudioDeviceReady()) applyVolume(it -> second);
}

void SoundManager :: setMasterVolume(float volume) {
    masterVolume = std :: clamp(volume, 0.0f, 1.0f);
    if(!IsAudioDeviceReady()) return;
    for(auto& [key, asset] : sounds) applyVolume(asset);
}

float SoundManager :: getMasterVolume() const {
    return masterVolume;
}

void SoundManager :: setMuted(bool shouldMute) {
    muted = shouldMute;
    if(!IsAudioDeviceReady()) return;
    for(auto& [key, asset] : sounds) applyVolume(asset);
}

bool SoundManager :: isMuted() const {
    return muted;
}

void SoundManager :: clear() {
    if(IsAudioDeviceReady()) {
        for(auto& [key, asset] : sounds) {
            StopSound(asset.sound);
            if(IsSoundValid(asset.sound)) UnloadSound(asset.sound);
        }
    }
    sounds.clear();
}
