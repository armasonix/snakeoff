#pragma once
#include "states/IGameState.h"
#include <vector>
#include <string>
#include <SFML/Audio.hpp>
#include <cmath>

class MenuState : public IGameState 
{
public:
    MenuState(class StateMachine& sm, sf::RenderWindow& win, class Config& cfg, class Resources& res);
    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;

private:
    StateMachine& sm_;
    sf::RenderWindow& win_;
    Config& cfg_;
    Resources& res_;
    int             selected_{ 0 };
    std::vector<std::string> items_{ "Start Game","Difficulty","Highlights","Settings","Quit" };

    sf::Sound sfxMove_;
    sf::Sound sfxHit_;

    sf::Text titleText_;
    float titleHue_{ 0.f };
    float titleHueSpeed_{ 60.f };
};