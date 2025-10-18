#include "Resources.h"

bool Resources::load(const std::string& assetsDir) 
{
    font_ = std::make_unique<sf::Font>();
    eat_ = std::make_unique<sf::SoundBuffer>();
    death_ = std::make_unique<sf::SoundBuffer>();
    bool ok = true;
    ok &= font_->loadFromFile(assetsDir + "/fonts/Roboto-Regular.ttf");
    ok &= eat_->loadFromFile(assetsDir + "/sfx/apple.wav");
    ok &= death_->loadFromFile(assetsDir + "/sfx/lose.wav");
    return ok;
}