#pragma once
#include <SFML/Audio/Sound.hpp>
#include <functional>

class Config;
class Snake;
class Score;

struct PlayContext
{
    Config& cfg;
    Snake& snake;
    Score& score;
    sf::Sound& sfxEat;
    std::function<void()> flash;
};