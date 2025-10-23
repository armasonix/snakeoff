#include "entities/Powerup.h"

void Powerup::spawn(const Vec2i& c, PowerupKind k, float ttl, int cellPx)
{
    cell = c;
    kind = k;
    ttlSec = ttl;
    alive = true;
    cellPx_ = cellPx;

    shape_.setSize(sf::Vector2f((float)cellPx_, (float)cellPx_));
    shape_.setFillColor(sf::Color(200, 40, 255));
    shape_.setOutlineThickness(2.f);
    shape_.setOutlineColor(sf::Color::White);
    syncShape();
}

void Powerup::update(float dt)
{
    if (!alive) return;
    ttlSec -= dt;
    if (ttlSec <= 0.f) 
    {
        alive = false;
        return;
    }

    // slightly flicker
    if (ttlSec < 4.f) 
    {
        float a = 120.f + 120.f * std::abs(std::sin(ttlSec * 8.f));
        auto col = shape_.getFillColor();
        col.a = (sf::Uint8)std::clamp(a, 0.f, 255.f);
        shape_.setFillColor(col);
    }
}

void Powerup::draw(sf::RenderTarget& rt, int cellPx) const
{
    if (!alive) return;
    if (cellPx_ != cellPx) 
    {
        const_cast<Powerup*>(this)->cellPx_ = cellPx;
        syncShape();
    }
    rt.draw(shape_);
}

void Powerup::syncShape() const
{
    shape_.setPosition((float)cell.x * cellPx_, (float)cell.y * cellPx_);
}