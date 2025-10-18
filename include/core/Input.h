#pragma once
#include <lib/SFML/Window/Keyboard.hpp>

struct Input 
{
    sf::Keyboard::Key up = sf::Keyboard::W;
    sf::Keyboard::Key down = sf::Keyboard::S;
    sf::Keyboard::Key left = sf::Keyboard::A;
    sf::Keyboard::Key right = sf::Keyboard::D;
    sf::Keyboard::Key enter = sf::Keyboard::Enter;
    sf::Keyboard::Key back = sf::Keyboard::B;
    sf::Keyboard::Key pause = sf::Keyboard::P;
};