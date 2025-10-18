#pragma once
#include <lib/SFML/Graphics.hpp>

struct IUpdatable 
{
    virtual ~IUpdatable() = default;
    virtual void update(float dt) = 0;
};

struct IRenderable 
{
    virtual ~IRenderable() = default;
    virtual void draw(sf::RenderTarget& rt) const = 0;
};