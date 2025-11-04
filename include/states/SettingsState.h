#pragma once
#include "states/IGameState.h"
#include "core/Config.h"
#include "core/Resources.h"
#include "core/StateMachine.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class SettingsState : public IGameState 
{
public:
    SettingsState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res);

    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;

private:
    enum Sel { Sound = 0, Music = 1, Eph = 2, Count = 3 };
    int selected_ = 0;

    struct Item 
    {
        const char* label;
        bool* value;
    };
    std::vector<Item> items_;

    StateMachine& sm_;
    sf::RenderWindow& win_;
    Config& cfg_;
    Resources& res_;

    void toggleCurrent();

    sf::Sound sfxMove_;
    sf::Sound sfxHit_;
};