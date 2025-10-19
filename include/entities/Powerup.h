#pragma once
#include <SFML/Graphics.hpp>
#include "util/Types.h"

enum class PowerupKind { Breaker };

class Powerup 
{
public:
    Powerup(const Vec2i& cell, float ttlSec, int cellPx)
        : kind_(PowerupKind::Breaker), cell_(cell), ttl_(ttlSec), cellPx_(cellPx) 
    {
        shape_.setRadius(cellPx * 0.4f);
        shape_.setOrigin(shape_.getRadius(), shape_.getRadius());
        shape_.setFillColor(sf::Color(255, 200, 50, 220));
        syncShape();
    }

    void update(float dt) { if (ttl_ > 0.f) ttl_ -= dt; }
    bool alive() const { return ttl_ > 0.f; }
    Vec2i cell() const { return cell_; }

    void draw(sf::RenderTarget& rt, const sf::View& view) const 
    {
        (void)view;
        rt.draw(shape_);
    }

private:
    void syncShape() 
    {
        shape_.setPosition(float(cell_.x * cellPx_ + cellPx_ / 2),
            float(cell_.y * cellPx_ + cellPx_ / 2));
    }

    PowerupKind kind_;
    Vec2i  cell_;
    float  ttl_{};
    int    cellPx_{};
    sf::CircleShape shape_;
};