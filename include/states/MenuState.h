#pragma once
#include "states/IGameState.h"
#include "core/StateMachine.h"
#include "core/Config.h"
#include "core/Resources.h"
#include <vector>
#include <string>

class MenuState : public IGameState 
{
public:
    MenuState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res);

    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;

private:
    StateMachine& sm_;
    sf::RenderWindow& win_;
    Config& cfg_;
    Resources& res_;
    int selected_ = 0;
    std::vector<std::string> items_{ "Start Game","Difficulty","Highlights","Settings","Quit" };
    void startGame();
    void changeDifficulty();
};