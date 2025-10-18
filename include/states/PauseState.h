#pragma once
#include "states/IGameState.h"
#include "core/StateMachine.h"
#include "core/Config.h"
#include "core/Resources.h"

class PauseState : public IGameState 
{
public:
    PauseState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
        : sm_(sm), win_(win), cfg_(cfg), res_(res) {}

    void handleEvent(const sf::Event& e) override 
    {
        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter) sm_.pop(); // continue
        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::B) sm_.pop();     // back = continue
    }
    void update(float) override {}
    void draw(sf::RenderTarget& rt) override 
    {
        sf::Text t("Pause\nEnter - continue\nB game", res_.font(), 28);
        t.setPosition(60, 60); t.setFillColor(sf::Color::Yellow);
        rt.draw(t);
    }
private:
    StateMachine& sm_;
    sf::RenderWindow& win_;
    Config& cfg_;
    Resources& res_;
};