#pragma once
#include "states/IGameState.h"
#include "core/Config.h"
#include "core/Resources.h"
#include "systems/HighScores.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class HighScoresState : public IGameState 
{
public:
    HighScoresState(class StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res);

    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float) override {}
    void draw(sf::RenderTarget& rt) override;

private:
    StateMachine& sm_;
    sf::RenderWindow& win_;
    Config& cfg_;
    Resources& res_;
    HighScores hs_;
    const std::string path_ = "data/highscores.tsv";

    sf::Sound sfxHit_;
};