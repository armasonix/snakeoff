#pragma once
#include <lib/SFML/Graphics.hpp>
#include "systems/Score.h"

class HUD 
{
public:
    HUD(const sf::Font& font);
    void draw(sf::RenderTarget& rt, const Score& score) const;
private:
    mutable sf::Text text_;
};