#include "ui/HUD.h"
#include <limits>

HUD::HUD(const sf::Font& font)
{
    text_.setFont(font);
    text_.setCharacterSize(20);
    text_.setFillColor(sf::Color::White);
    text_.setOutlineThickness(0.0f);
}

void HUD::draw(sf::RenderTarget& rt, const Score& score) const
{
    const int v = score.value();
    if (v != lastValue_)
    {
        text_.setString("Score: " + std::to_string(v));
        lastValue_ = v;
    }
    text_.setFillColor(color_);

    const auto prev = rt.getView();
    rt.setView(rt.getDefaultView());

    const auto b = text_.getLocalBounds();
    const auto size = rt.getView().getSize();
    text_.setPosition(size.x - margin_ - b.width, margin_);

    rt.draw(text_);
    rt.setView(prev);
}