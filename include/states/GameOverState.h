#pragma once
#include "states/IGameState.h"
#include "core/Config.h"
#include "core/Resources.h"
#include "systems/HighScores.h"
#include <string>
#include <SFML/Audio.hpp>

class GameOverState : public IGameState 
{
public:
    GameOverState(class StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res, int score)
        : sm_(sm), win_(win), cfg_(cfg), res_(res), score_(score) {}

    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;

private:
    enum class Phase { AskSave, EnterName, Options };

    StateMachine& sm_;
    sf::RenderWindow& win_;
    Config& cfg_;
    Resources& res_;
    int score_{ 0 };

    HighScores hs_;
    const std::string path_ = "data/highscores.tsv";

    // AskSave
    int askSel_{ 0 }; // 0=no, 1=yes

    // EnterName
    std::string name_{ "ABC" };

    // Options
    int optSel_{ 0 }; // 0=start, 1=to main menu

    Phase phase_{ Phase::AskSave };

    sf::Sound sfxMove_;
    sf::Sound sfxHit_;

    float titleHue_{ 0.f };
    float titleHueSpeed_{ 60.f };
};