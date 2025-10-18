#pragma once
#include "entities/Entity.h"
#include "util/Types.h"
#include <lib/SFML/Graphics.hpp>

class PlayContext;

class Apple : public IRenderable 
{
public:
    explicit Apple(const Vec2i& cell) : cell_(cell) {}
    virtual ~Apple() = default;

    Vec2i cell() const { return cell_; }

    virtual void onEaten(PlayContext& ctx) = 0;

    // render
    void draw(sf::RenderTarget& rt) const override;

protected:
    Vec2i cell_;
};