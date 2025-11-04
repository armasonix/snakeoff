#pragma once
#include <SFML/System/Clock.hpp>

namespace perf
{
    struct ScopeTimer
    {
        explicit ScopeTimer(float& outMs) : out(outMs) { out = 0.f; }
        ~ScopeTimer() { out = clock.getElapsedTime().asSeconds() * 1000.f; }
    private:
        sf::Clock clock;
        float& out;
    };

    struct FpsCounter
    {
        void tick() noexcept
        {
            ++frames_;
            const float s = acc_.getElapsedTime().asSeconds();
            if (s >= intervalSec_) 
            {
                fps_ = frames_ / s;
                frames_ = 0;
                acc_.restart();
            }
        }

        float fps()     const noexcept { return fps_; }
        float frameMs() const noexcept { return fps_ > 0.f ? 1000.f / fps_ : 0.f; }
        void  setInterval(float seconds) noexcept { intervalSec_ = seconds; }

    private:
        sf::Clock acc_;
        int   frames_ = 0;
        float fps_ = 0.f;
        float intervalSec_ = 0.5f;
    };
}