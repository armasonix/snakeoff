#pragma once
#include "states/IGameState.h"
#include "core/StateMachine.h"
#include "core/Config.h"
#include "core/Resources.h"
#include <SFML/Graphics.hpp>

class PauseState : public IGameState 
{
public:
    PauseState(class StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
        : sm_(sm), win_(win), cfg_(cfg), res_(res) {}

    void handleEvent(const sf::Event& e) override 
    {
        if (e.type == sf::Event::KeyPressed) 
        {
            if (e.key.code == sf::Keyboard::Enter || e.key.code == sf::Keyboard::B) 
            {
                sm_.pop();
            }
        }
    }
    void update(float) override {}
    void draw(sf::RenderTarget& rt) override 
    {
        sf::Text t("Pause\nEnter/B - continue", res_.font(), 28);
        t.setPosition(60, 60); t.setFillColor(sf::Color::Yellow);
        rt.draw(t);
    }
private:
    StateMachine& sm_;
    sf::RenderWindow& win_;
    Config& cfg_;
    Resources& res_;
};