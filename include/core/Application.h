#pragma once
#include <lib/SFML/Graphics.hpp>
#include "core/StateMachine.h"
#include "core/Config.h"
#include "core/Resources.h"

class Application 
{
public:
    Application(int wPx, int hPx);
    bool init();
    void run();

private:
    sf::RenderWindow win_;
    StateMachine sm_;
    Config cfg_;
    Resources res_;
};