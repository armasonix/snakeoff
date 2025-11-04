#pragma once
#include <SFML/Graphics.hpp>
#include "systems/Score.h"

class HUD
{
public:
    HUD(const sf::Font& font);

    void draw(sf::RenderTarget& rt, const Score& score) const;

    void setMargin(float px) { margin_ = px; }
    float margin() const { return margin_; }
    void setColor(const sf::Color& c) { color_ = c; }

private:
    mutable sf::Text text_;
    mutable int lastValue_ = std::numeric_limits<int>::min();
    float  margin_ = 8.f;
    sf::Color color_ = sf::Color::White;
};