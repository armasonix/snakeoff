#pragma once
#include "core/State.h"
#include <SFML/Graphics.hpp>

struct IGameState : public IState 
{
    virtual void handleEvent(const sf::Event& e) = 0;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderTarget& rt) = 0;
};