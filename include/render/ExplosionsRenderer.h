#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
namespace render { struct RenderStats; }

namespace render
{
    struct RenderStats;
    void drawExplosions(sf::RenderTarget & world,
        const sf::Sprite & sprExpl,
        const std::vector<sf::Vector2f>&positions,
        const std::vector<float>&times,
        float lifeSeconds = 0.25f,
        RenderStats * stats = nullptr);
}