#include "states/GameOverState.h"
#include "states/PlayState.h"
#include "states/MenuState.h"
#include <SFML/Graphics.hpp>

void GameOverState::handleEvent(const sf::Event& e) 
{
    if (e.type == sf::Event::KeyPressed) 
    {
        if (e.key.code == sf::Keyboard::W || e.key.code == sf::Keyboard::S)
            selected_ = 1 - selected_;
        if (e.key.code == sf::Keyboard::Enter) 
        {
            if (selected_ == 0) sm_.push(std::make_unique<PlayState>(sm_, win_, cfg_, res_));
            else               sm_.push(std::make_unique<MenuState>(sm_, win_, cfg_, res_));
        }
    }
}
void GameOverState::draw(sf::RenderTarget& rt) 
{
    sf::Text t; t.setFont(res_.font()); t.setCharacterSize(28);
    t.setString("Game Over!\nScore: " + std::to_string(score_) + "\n\nEnter - select");
    t.setPosition(60, 60); t.setFillColor(sf::Color::White); rt.draw(t);

    sf::Text a("Start Game", res_.font(), 24);
    sf::Text b("Main Menu", res_.font(), 24);
    a.setPosition(60, 160); b.setPosition(60, 200);
    a.setFillColor(selected_ == 0 ? sf::Color::Green : sf::Color::White);
    b.setFillColor(selected_ == 1 ? sf::Color::Green : sf::Color::White);
    rt.draw(a); rt.draw(b);
}