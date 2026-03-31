#pragma once

#include "miniaudio.h"
#include <string>
#include <mutex>

class SoundPlayer
{
private:
    ma_engine engine_;
    ma_sound current_sound_;
    bool initialized_ = false;
    mutable std::mutex mutex_;

public:
    SoundPlayer();
    ~SoundPlayer();

    SoundPlayer(const SoundPlayer &) = delete;
    SoundPlayer &operator=(const SoundPlayer &) = delete;

    bool isInitialized() const { return initialized_; }

    bool play(const std::string &filepath, bool loop = false);
    void stop();
    void set_volume(float volume);
    bool is_playing() const;
};