#include <Core/MusicManager.hpp>
#include <cctype>

MusicManager :: ~MusicManager() {
    clear();
}

std :: string MusicManager :: normalizeKey(const std :: string& key) const {
    std :: string normalized = key;
    for(char& character : normalized) {
        character = static_cast<char>(std :: toupper(
            static_cast<unsigned char>(character)
        ));
    }
    return normalized;
}

void MusicManager :: applyVolume(MusicAsset& asset) const {
    const float outputVolume = muted ? 0.0f : asset.volume * masterVolume;
    SetMusicVolume(asset.music, std :: clamp(outputVolume, 0.0f, 1.0f));
}

bool MusicManager :: addMusic(const std :: string& key, const std :: string& path) {
    if(!IsAudioDeviceReady()) {
        TraceLog(LOG_WARNING, "MusicManager: audio device is not ready");
        return false;
    }

    Music loadedMusic = LoadMusicStream(path.c_str());
    if(!IsMusicValid(loadedMusic)) {
        TraceLog(LOG_WARNING, "MusicManager: cannot load '%s'", path.c_str());
        return false;
    }

    const std :: string normalized = normalizeKey(key);
    removeMusic(normalized);
    tracks[normalized] = {loadedMusic, 1.0f};
    applyVolume(tracks[normalized]);
    return true;
}

bool MusicManager :: removeMusic(const std :: string& key) {
    const std :: string normalized = normalizeKey(key);
    auto it = tracks.find(normalized);
    if(it == tracks.end()) return false;

    if(currentTrack == normalized) stop();
    if(IsAudioDeviceReady() && IsMusicValid(it -> second.music)) {
        UnloadMusicStream(it -> second.music);
    }
    tracks.erase(it);
    return true;
}

bool MusicManager :: hasMusic(const std :: string& key) const {
    return tracks.find(normalizeKey(key)) != tracks.end();
}

bool MusicManager :: play(const std :: string& key, bool loop, bool restart) {
    const std :: string normalized = normalizeKey(key);
    auto it = tracks.find(normalized);
    if(it == tracks.end() || !IsAudioDeviceReady()) return false;

    if(currentTrack == normalized && !restart) {
        if(paused) resume();
        return true;
    }

    stop();
    currentTrack = normalized;
    paused = false;
    it -> second.music.looping = loop;
    applyVolume(it -> second);
    PlayMusicStream(it -> second.music);
    return true;
}

void MusicManager :: update() {
    if(currentTrack.empty() || !IsAudioDeviceReady()) return;

    auto it = tracks.find(currentTrack);
    if(it == tracks.end()) {
        currentTrack.clear();
        paused = false;
        return;
    }

    UpdateMusicStream(it -> second.music);
    if(!paused && !it -> second.music.looping &&
       !IsMusicStreamPlaying(it -> second.music)) {
        currentTrack.clear();
    }
}

void MusicManager :: stop() {
    if(currentTrack.empty()) return;

    auto it = tracks.find(currentTrack);
    if(it != tracks.end() && IsAudioDeviceReady()) StopMusicStream(it -> second.music);
    currentTrack.clear();
    paused = false;
}

void MusicManager :: pause() {
    if(currentTrack.empty() || paused || !IsAudioDeviceReady()) return;
    auto it = tracks.find(currentTrack);
    if(it == tracks.end()) return;

    PauseMusicStream(it -> second.music);
    paused = true;
}

void MusicManager :: resume() {
    if(currentTrack.empty() || !paused || !IsAudioDeviceReady()) return;
    auto it = tracks.find(currentTrack);
    if(it == tracks.end()) return;

    ResumeMusicStream(it -> second.music);
    paused = false;
}

void MusicManager :: seek(float seconds) {
    if(currentTrack.empty() || !IsAudioDeviceReady()) return;
    auto it = tracks.find(currentTrack);
    if(it == tracks.end()) return;

    const float length = GetMusicTimeLength(it -> second.music);
    SeekMusicStream(it -> second.music, std :: clamp(seconds, 0.0f, length));
}

bool MusicManager :: isPlaying() const {
    if(currentTrack.empty() || paused || !IsAudioDeviceReady()) return false;
    auto it = tracks.find(currentTrack);
    return it != tracks.end() && IsMusicStreamPlaying(it -> second.music);
}

bool MusicManager :: isPaused() const {
    return paused;
}

std :: string MusicManager :: getCurrentTrack() const {
    return currentTrack;
}

float MusicManager :: getTimePlayed() const {
    auto it = tracks.find(currentTrack);
    return it == tracks.end() ? 0.0f : GetMusicTimePlayed(it -> second.music);
}

float MusicManager :: getTimeLength() const {
    auto it = tracks.find(currentTrack);
    return it == tracks.end() ? 0.0f : GetMusicTimeLength(it -> second.music);
}

void MusicManager :: setMusicVolume(const std :: string& key, float volume) {
    auto it = tracks.find(normalizeKey(key));
    if(it == tracks.end()) return;

    it -> second.volume = std :: clamp(volume, 0.0f, 1.0f);
    if(IsAudioDeviceReady()) applyVolume(it -> second);
}

void MusicManager :: setMasterVolume(float volume) {
    masterVolume = std :: clamp(volume, 0.0f, 1.0f);
    if(!IsAudioDeviceReady()) return;
    for(auto& [key, asset] : tracks) applyVolume(asset);
}

float MusicManager :: getMasterVolume() const {
    return masterVolume;
}

void MusicManager :: setMuted(bool shouldMute) {
    muted = shouldMute;
    if(!IsAudioDeviceReady()) return;
    for(auto& [key, asset] : tracks) applyVolume(asset);
}

bool MusicManager :: isMuted() const {
    return muted;
}

void MusicManager :: clear() {
    stop();
    if(IsAudioDeviceReady()) {
        for(auto& [key, asset] : tracks) {
            if(IsMusicValid(asset.music)) UnloadMusicStream(asset.music);
        }
    }
    tracks.clear();
}
