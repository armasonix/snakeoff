#pragma once
#include "states/IGameState.h"
#include "core/Config.h"
#include "core/Resources.h"
#include "core/StateMachine.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class DifficultyState : public IGameState
{
public:
    DifficultyState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res);

    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;

private:
    struct Entry 
    {
        Difficulty   d;
        const char* name;
        const char* tagline;
    };

    std::vector<Entry> items_;
    int selected_ = 0;

    StateMachine& sm_;
    sf::RenderWindow& win_;
    Config& cfg_;
    Resources& res_;

    int indexFromDifficulty(Difficulty d) const;

    sf::Sound sfxMove_;
    sf::Sound sfxHit_;
};