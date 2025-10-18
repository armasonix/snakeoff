#include "states/MenuState.h"
#include "states/PlayState.h"
#include "core/StateMachine.h" 
#include <SFML/Graphics.hpp>
#include "states/HighScoresState.h"

MenuState::MenuState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
    : sm_(sm), win_(win), cfg_(cfg), res_(res) {}

void MenuState::onEnter() {}

void MenuState::handleEvent(const sf::Event& e) 
{
    if (e.type == sf::Event::KeyPressed) 
    {
        if (e.key.code == sf::Keyboard::W) selected_ = (selected_ + (int)items_.size() - 1) % (int)items_.size();
        if (e.key.code == sf::Keyboard::S) selected_ = (selected_ + 1) % (int)items_.size();
        if (e.key.code == sf::Keyboard::Enter) 
        {
            if (selected_ == 0) startGame();
            else if (selected_ == 1) changeDifficulty();
            else if (selected_ == 2) sm_.push(std::make_unique<HighScoresState>(sm_, win_, cfg_, res_)); // highlights
            else if (selected_ == 4) win_.close();
        }
    }
}
void MenuState::update(float) {}

void MenuState::draw(sf::RenderTarget& rt) 
{
    sf::Text t; t.setFont(res_.font()); t.setCharacterSize(28);
    for (int i = 0;i < (int)items_.size();++i) 
    {
        t.setString(items_[i]);
        t.setFillColor(i == selected_ ? sf::Color::Green : sf::Color::White);
        t.setPosition(60.f, 80.f + i * 40.f);
        rt.draw(t);
    }
    sf::Text d; d.setFont(res_.font()); d.setCharacterSize(18);
    d.setPosition(60.f, 40.f);
    d.setString("Difficulty: " + std::to_string((int)cfg_.difficulty));
    rt.draw(d);
}

void MenuState::startGame() 
{
    // change current state for game
    sm_.push(std::make_unique<class PlayState>(sm_, win_, cfg_, res_));
}

void MenuState::changeDifficulty() 
{
    int cur = (int)cfg_.difficulty;
    cur = (cur % 5) + 1;
    cfg_.difficulty = (Difficulty)cur;
}