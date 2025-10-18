#include "ui/HUD.h"

HUD::HUD(const sf::Font& font) { text_.setFont(font); text_.setCharacterSize(20); }

void HUD::draw(sf::RenderTarget& rt, const Score& score) const 
{
    text_.setString("Score: " + std::to_string(score.value()));
    text_.setPosition(rt.getView().getSize().x - 150.f, 8.f);
    text_.setFillColor(sf::Color::White);
    rt.draw(text_);
}