#pragma once
#include "states/IGameState.h"
#include "core/Config.h"
#include "core/Resources.h"
#include "world/Level.h"
#include "entities/Snake.h"
#include "entities/Apple.h"
#include "systems/Score.h"
#include "ui/HUD.h"
#include "vfx/CameraShake.h"

#include <memory>
#include <functional>

struct PlayContext;

class PlayState : public IGameState 
{
public:
    PlayState(class StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res);

    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;

    Snake& snake() { return snake_; }
    Score& score() { return score_; }
    Config& config() { return cfg_; }

    void flashSnake();

private:
    StateMachine& sm_;
    sf::RenderWindow& win_;
    Config& cfg_;
    Resources& res_;

    Level                   level_;
    Snake                   snake_;
    std::unique_ptr<Apple>  apple_;
    Score                   score_;
    HUD                     hud_;
    CameraShake             shake_;

    sf::Sound               sfxEat_, sfxDeath_;
    float                   timeAcc_{ 0.f };
    float                   startDelay_{ 0.f };

    void spawnApple();
    void die();
};

struct PlayContext 
{
    Config& config;
    Snake& snake;
    Score& score;
    sf::Sound& sfxEat;
    std::function<void()> flashSnake;
};