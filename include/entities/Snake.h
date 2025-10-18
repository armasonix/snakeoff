#pragma once
#include "entities/Entity.h"
#include "util/Types.h"
#include <deque>
#include <optional>

class Snake : public IUpdatable, public IRenderable 
{
public:
    explicit Snake(const Vec2i& startCell);

    void setDirection(Direction d);
    Direction direction() const { return dir_; }

    void grow(int cells) { growth_ += cells; }
    const std::deque<Vec2i>& body() const { return body_; }
    Vec2i head() const { return body_.front(); }

    // move step
    void step();

    // IUpdatable (vfx)
    void update(float dt) override;

    // IRenderable
    void draw(sf::RenderTarget& rt) const override;

    bool bitesItself() const; // suicide

    void teleportHead(const Vec2i& cell);

private:
    std::deque<Vec2i> body_;
    Direction dir_{ Direction::Right };
    int growth_{ 0 };
    float flashTimer_{ 0.f }; // flicker
};