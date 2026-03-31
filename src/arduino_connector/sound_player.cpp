// sound_player.cpp
#define MA_IMPLEMENTATION
#include "miniaudio.h"
#include "sound_player.hpp"
#include <cstdio>
#include <cstring>
#include <syslog.h>

SoundPlayer::SoundPlayer()
{
    std::memset(&current_sound_, 0, sizeof(ma_sound));

    ma_result result = ma_engine_init(nullptr, &engine_);
    if (result != MA_SUCCESS)
    {
        syslog(LOG_ERR, "SoundPlayer: ошибка инициализации: %d", result);
        initialized_ = false;
        return;
    }
    else
    {
        initialized_ = true;
        syslog(LOG_INFO, "SoundPlayer: инициализирован");
    }
}

SoundPlayer::~SoundPlayer()
{
    if (initialized_)
    {
        stop();
        ma_engine_uninit(&engine_);
        syslog(LOG_INFO, "SoundPlayer: остановлен");
    }
}

bool SoundPlayer::play(const std::string &filepath, bool loop)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_)
    {
        syslog(LOG_ERR, "SoundPlayer: не инициализирован");
        return false;
    }

    if (ma_sound_is_playing(&current_sound_))
    {
        ma_sound_stop(&current_sound_);
        ma_sound_uninit(&current_sound_);
    }

    std::memset(&current_sound_, 0, sizeof(ma_sound));

    ma_uint32 flags = loop ? MA_SOUND_FLAG_LOOPING : 0;
    ma_result result = ma_sound_init_from_file(
        &engine_,
        filepath.c_str(),
        flags,
        nullptr,
        nullptr,
        &current_sound_);

    if (result != MA_SUCCESS)
    {
        syslog(LOG_ERR, "SoundPlayer: ошибка загрузки '%s': %d", filepath.c_str(), result);
        return false;
    }

    ma_sound_start(&current_sound_);
    syslog(LOG_DEBUG, "SoundPlayer: запущен '%s'", filepath.c_str());
    return true;
}

void SoundPlayer::stop()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (ma_sound_is_playing(&current_sound_))
    {
        ma_sound_stop(&current_sound_);
        ma_sound_uninit(&current_sound_);
        std::memset(&current_sound_, 0, sizeof(ma_sound));
        syslog(LOG_DEBUG, "SoundPlayer: остановлен");
    }
}

void SoundPlayer::set_volume(float volume)
{
    std::lock_guard<std::mutex> lock(mutex_);
    ma_sound_set_volume(&current_sound_, volume);
}

bool SoundPlayer::is_playing() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return initialized_ && ma_sound_is_playing(&current_sound_);
}