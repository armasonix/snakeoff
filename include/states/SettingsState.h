#pragma once
#include "states/IGameState.h"
#include "core/Config.h"
#include "core/Resources.h"
#include "core/StateMachine.h"
#include <SFML/Graphics.hpp>

class SettingsState : public IGameState 
{
public:
    SettingsState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res);

    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;

private:
    enum Sel { Sound = 0, Music = 1, Count = 2 };
    int selected_ = 0;

    StateMachine& sm_;
    sf::RenderWindow& win_;
    Config& cfg_;
    Resources& res_;

    void toggleCurrent();
};