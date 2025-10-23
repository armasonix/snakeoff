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

    bool canBreakObstacles() const noexcept { return breakObstacles_; }
    void enableBreaker(float seconds) noexcept 
    {
        breakerTTL_ = std::max(breakerTTL_, seconds);
        breakObstacles_ = true;
    }

    float breakerTimeLeft() const noexcept { return breakerTTL_; }

    void tickBreaker(float dt) noexcept 
    {
        if (breakerTTL_ > 0.f) 
        {
            breakerTTL_ -= dt;
            if (breakerTTL_ <= 0.f) 
            {
                breakerTTL_ = 0.f;
                breakObstacles_ = false;
            }
        }
    }

private:
    std::deque<Vec2i> body_;
    Direction dir_{ Direction::Right };
    int growth_{ 0 };
    float flashTimer_{ 0.f }; // flicker
    bool breakObstacles_{ false };
    float breakerTTL_{ 0.f };
};