#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <string>

class Resources 
{
public:
    bool load(const std::string& assetsDir);
    const sf::Font& font() const { return *font_; }
    const sf::SoundBuffer& sfxEat() const { return *eat_; }
    const sf::SoundBuffer& sfxDeath() const { return *death_; }
private:
    std::unique_ptr<sf::Font> font_;
    std::unique_ptr<sf::SoundBuffer> eat_, death_;
};