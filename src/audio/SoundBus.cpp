#include "audio/SoundBus.h"
#include <algorithm>

namespace audio
{
    SoundBus::SoundBus(std::size_t capacity, float masterVolume)
        : voices_(capacity), masterVolume_(std::clamp(masterVolume, 0.f, 100.f))
    {
        for (auto& v : voices_) v.setVolume(masterVolume_);
    }

    void SoundBus::setMasterVolume(float v)
    {
        masterVolume_ = std::clamp(v, 0.f, 100.f);
        for (auto& s : voices_) s.setVolume(masterVolume_);
    }

    float SoundBus::masterVolume() const { return masterVolume_; }

    void SoundBus::setCapacity(std::size_t capacity)
    {
        if (capacity == voices_.size()) return;
        stopAll();
        voices_.assign(capacity, sf::Sound{});
        for (auto& v : voices_) v.setVolume(masterVolume_);
        next_ = 0;
    }

    std::size_t SoundBus::capacity() const { return voices_.size(); }

    void SoundBus::stopAll()
    {
        for (auto& s : voices_) s.stop();
    }

    sf::Sound& SoundBus::acquire_()
    {
        // loop bufer
        sf::Sound& s = voices_[next_];
        next_ = (next_ + 1) % voices_.size();
        if (s.getStatus() == sf::Sound::Playing) s.stop();
        return s;
    }

    void SoundBus::play(const sf::SoundBuffer& buffer,
        const std::string& tag,
        float minIntervalSec,
        float volume01,
        float pitch)
    {
        const float now = clock_.getElapsedTime().asSeconds();
        if (!tag.empty())
        {
            const auto it = lastPlaySec_.find(tag);
            if (it != lastPlaySec_.end())
            {
                if ((now - it->second) < std::max(0.f, minIntervalSec))
                    return;
            }
            lastPlaySec_[tag] = now;
        }

        sf::Sound& s = acquire_();
        s.setBuffer(buffer);
        s.setPitch(std::max(0.01f, pitch));
        s.setVolume(std::clamp(volume01, 0.f, 1.f) * masterVolume_);
        s.play();
    }

    void SoundBus::resetRateLimit()
    {
        lastPlaySec_.clear();
        clock_.restart();
    }
}