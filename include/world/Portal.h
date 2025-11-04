#pragma once
#include <SFML/Graphics/Color.hpp>
#include "util/Types.h" 

struct PortalPair 
{
    Vec2i    a;   // enter
    Vec2i    b;   // exit
    sf::Color color{ 100, 200, 255 };
};