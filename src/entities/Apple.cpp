#include "entities/Apple.h"
#include <SFML/Graphics.hpp>

void Apple::draw(sf::RenderTarget& rt) const 
{
    const int CELL = 24;
    sf::CircleShape cir((float)CELL * 0.45f);
    cir.setOrigin(cir.getRadius(), cir.getRadius());
    cir.setFillColor(sf::Color(220, 0, 0));
    cir.setPosition((float)cell_.x * CELL + CELL * 0.5f, (float)cell_.y * CELL + CELL * 0.5f);
    rt.draw(cir);
}