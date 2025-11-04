#include "entities/Snake.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

Snake::Snake(const Vec2i& startCell) 
{
    body_.push_front(startCell);
    body_.push_back({ startCell.x - 1,startCell.y });
    body_.push_back({ startCell.x - 2,startCell.y });
}

void Snake::setDirection(Direction d) 
{
    // 180 deg restrict
    if ((dir_ == Direction::Up && d == Direction::Down) ||
        (dir_ == Direction::Down && d == Direction::Up) ||
        (dir_ == Direction::Left && d == Direction::Right) ||
        (dir_ == Direction::Right && d == Direction::Left)) return;
    dir_ = d;
}

void Snake::step() 
{
    Vec2i delta{ 0,0 };
    switch (dir_) 
    {
    case Direction::Up:    delta = { 0,-1 }; break;
    case Direction::Down:  delta = { 0, 1 }; break;
    case Direction::Left:  delta = { -1,0 }; break;
    case Direction::Right: delta = { 1,0 }; break;
    }
    Vec2i newHead = body_.front() + delta;
    body_.push_front(newHead);
    if (growth_ > 0) --growth_;
    else body_.pop_back();
}

void Snake::teleportHead(const Vec2i& cell)
{
    if (body_.empty()) return;
    body_[0] = cell;
}

void Snake::update(float dt) 
{
    if (flashTimer_ > 0.f) flashTimer_ = std::max(0.f, flashTimer_ - dt);
}

void Snake::draw(sf::RenderTarget& rt) const 
{
    const int CELL = 24; // parse from Config in PlayState and forward
    sf::RectangleShape rect({ (float)CELL - 1,(float)CELL - 1 });
    for (size_t i = 0;i < body_.size();++i) 
    {
        const auto& c = body_[i];
        rect.setPosition((float)c.x * CELL, (float)c.y * CELL);
        if (i == 0) rect.setFillColor(sf::Color(0, 220, 0)); // head
        else 
        {
            // easy flick
            sf::Color col = sf::Color(0, 160, 0);
            if (flashTimer_ > 0.f) col.a = (sf::Uint8)(120 + 135 * std::fabs(std::sin(flashTimer_ * 20.f)));
            rect.setFillColor(col);
        }
        rt.draw(rect);
    }
}

bool Snake::bitesItself() const 
{
    const auto H = head();
    for (size_t i = 1;i < body_.size();++i)
        if (body_[i] == H) return true;
    return false;
}