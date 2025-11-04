#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

namespace render
{
    inline void drawGate(sf::RenderTarget& rt,
        bool gateUnlocked,
        sf::RectangleShape& gateViz,
        float& gatePulse)
    {
        if (!gateUnlocked) return;
        gatePulse += 0.8f * (1.f / 60.f);
        const float a = 180.f + 60.f * std::sin(gatePulse * 6.2831853f);
        auto col = gateViz.getFillColor();
        col.a = static_cast<sf::Uint8>(std::clamp<int>(static_cast<int>(a), 0, 255));
        gateViz.setFillColor(col);
        rt.draw(gateViz);
    }
}