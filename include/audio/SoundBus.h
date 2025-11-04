#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <unordered_map>
#include <string>

namespace audio
{
    class SoundBus
    {
    public:
        explicit SoundBus(std::size_t capacity = 16, float masterVolume = 100.f);
        void setMasterVolume(float v);
        float masterVolume() const;

        void setCapacity(std::size_t capacity);
        std::size_t capacity() const;

        void stopAll();

        void play(const sf::SoundBuffer& buffer,
            const std::string& tag = std::string(),
            float minIntervalSec = 0.05f,
            float volume01 = 1.0f,
            float pitch = 1.0f);

        void resetRateLimit();

    private:
        std::vector<sf::Sound> voices_;
        std::size_t next_ = 0;

        sf::Clock clock_;
        std::unordered_map<std::string, float> lastPlaySec_;
        float masterVolume_ = 100.f;

        sf::Sound& acquire_();
    };
}