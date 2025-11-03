#pragma once
#include <SFML/System/Clock.hpp>

namespace perf
{
    struct ScopeTimer
    {
        explicit ScopeTimer(float& outMs) : out(outMs) { out = 0.f; }
        ~ScopeTimer() { out = clock.getElapsedTime().asMilliseconds(); }
    private:
        sf::Clock clock;
        float& out;
    };
}