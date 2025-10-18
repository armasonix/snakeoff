#pragma once
#include "states/IGameState.h"
#include "core/StateMachine.h"
#include "core/Config.h"
#include "core/Resources.h"

class GameOverState : public IGameState 
{
public:
    GameOverState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res, int score)
        : sm_(sm), win_(win), cfg_(cfg), res_(res), score_(score) {}

    void handleEvent(const sf::Event& e) override;
    void update(float) override {}
    void draw(sf::RenderTarget& rt) override;

private:
    StateMachine& sm_;
    sf::RenderWindow& win_;
    Config& cfg_;
    Resources& res_;
    int score_;
    int selected_ = 0; // 0 - start, 1 - menu
};