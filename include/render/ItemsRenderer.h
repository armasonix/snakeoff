#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "entities/Powerup.h"

class Config;
class Apple;

namespace render
{
    enum class AppleKind { Normal, Bonus, Poison, Confuse };

    void drawItems(sf::RenderTarget& world,
        const Config& cfg,
        const Apple* apple,
        AppleKind    kind,
        float        appleTTL,
        const std::vector<PowerUp>& powerups,
        sf::Sprite& sprApple1,
        sf::Sprite& sprApple2,
        sf::Sprite& sprApple3,
        sf::Sprite& sprPowerBomb,
        sf::Sprite& sprPowerMush);
}