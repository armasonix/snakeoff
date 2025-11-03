#pragma once
#include <SFML/Graphics.hpp>
#include <array>
class Snake;

namespace render
{
    void drawSnake(sf::RenderTarget& world,
        const Snake& snake,
        int cellPx,
        sf::Sprite& sprHead,
        sf::Sprite& sprBody,
        sf::Sprite& sprTail,
        const std::array<sf::Sprite*, 4>& sprBodyCorner);
}