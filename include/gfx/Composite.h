#pragma once
#include <SFML/Graphics.hpp>

namespace gfx
{
    void drawChromAbComposite(sf::RenderTarget& rt,
        sf::RenderTexture& src,
        sf::Shader& shader,
        const sf::Vector2f& resPx,
        float time,
        float amount,
        const sf::View& worldView,
        const sf::Vector2f& cachedRes,
        float cachedAmount,
        sf::Vector2f* outCachedRes,
        float* outCachedAmount);
}