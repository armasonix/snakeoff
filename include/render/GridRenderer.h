#pragma once
#include <SFML/Graphics.hpp>
class Level;

namespace render
{
    void drawGrid(sf::RenderTarget& world,
        const Level& level,
        int cellPx,
        sf::Sprite& sprWall,
        sf::Sprite& sprObs);
}