#pragma once
#include <SFML/Graphics.hpp>
#include <array>

namespace render
{
    struct SpriteRefs
    {
        // snake
        sf::Sprite& head;
        sf::Sprite& body;
        sf::Sprite& tail;
        std::array<sf::Sprite*, 4> corners;

        // items
        sf::Sprite& apple1;
        sf::Sprite& apple2;
        sf::Sprite& apple3;
        sf::Sprite& powerBomb;
        sf::Sprite& powerMush;

        // others
        sf::Sprite& explosion;
        sf::Sprite& wall;
        sf::Sprite& obs;
    };
}