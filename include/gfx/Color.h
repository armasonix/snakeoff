#pragma once
#include <SFML/Graphics/Color.hpp>
#include <cmath>

namespace gfx
{
    inline sf::Color hsv(float h, float s, float v)
    {
        h = std::fmod(h, 360.f); if (h < 0) h += 360.f;
        const float c = v * s;
        const float x = c * (1.f - std::fabs(std::fmod(h / 60.f, 2.f) - 1.f));
        const float m = v - c;
        float r = 0.f, g = 0.f, b = 0.f;
        if (h < 60) { r = c; g = x; b = 0; }
        else if (h < 120) { r = x; g = c; b = 0; }
        else if (h < 180) { r = 0; g = c; b = x; }
        else if (h < 240) { r = 0; g = x; b = c; }
        else if (h < 300) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }
        return sf::Color(
            static_cast<sf::Uint8>(std::round((r + m) * 255.f)),
            static_cast<sf::Uint8>(std::round((g + m) * 255.f)),
            static_cast<sf::Uint8>(std::round((b + m) * 255.f)));
    }
}