#pragma once
#include <SFML/Graphics.hpp>
#include "math/Vec2.h"

enum class PowerupKind { Breaker };

struct Powerup
{
    PowerupKind kind{ PowerupKind::Breaker };
    bool  alive{ false };
    Vec2i cell{};          // cell on grid
    float ttlSec{ 0.f };   // ttl

    // spawn/respawn
    void spawn(const Vec2i& c, PowerupKind k, float ttl, int cellPx);

    // tick
    void update(float dt);

    // draw
    void draw(sf::RenderTarget& rt, int cellPx) const;

private:
    mutable sf::RectangleShape shape_;
    int cellPx_{ 24 };

    void syncShape() const; // position shape_ from cell/cellPx_
};