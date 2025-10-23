#pragma once
#include "states/IGameState.h"
#include "core/Config.h"
#include "core/Resources.h"
#include "world/Level.h"
#include "entities/Snake.h"
#include "entities/Apple.h"
#include "entities/Powerup.h"
#include "systems/Score.h"
#include "systems/Spawner.h"
#include "ui/HUD.h"
#include "vfx/CameraShake.h"
#include "systems/Effects.h"
#include <memory>
#include <random>
#include <functional>
#include "world/Portal.h"
#include <vector>
#include <algorithm>

struct PlayContext;

enum class AppleKind { Normal, Bonus, Poison, Confuse };

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
    float                   portalPulseT_ = 0.f;

    // rgb colorout
    float confuseVisT_ = 0.f;
    float confuseHueT_ = 0.f;

    Effects    effects_;
    AppleKind  appleKind_{ AppleKind::Normal };
    float      appleTTL_{ 0.f };

    void spawnApple();
    void die();

    bool initialized_ = false;
    std::mt19937 rng_{ std::random_device{}() };
    Spawner spawner_;

    std::unique_ptr<Powerup> powerup_; 
    float breakerTimer_ = 0.f;

    // EPH: cache of generated obstacles, and toggle state
    std::vector<sf::Vector2i> ephObstacles_;
    std::vector<sf::Vector2i> tempObstacles_;
    std::vector<Vec2i> ephCells_;
    bool   ephVisible_ = true;
    float  ephTimer_ = 0.f;

    // EPH: helpers to apply/hide without regenerating
    void ephApplyVisible();
    void ephApplyHidden();

    std::vector<PortalPair> portals_;
    Vec2i portalLockCell_{ -9999, -9999 }; // cell-lock

    void spawnPortals(int pairs = 1);
    bool isPortalCell(const Vec2i& c, size_t* outPairIdx = nullptr, bool* isA = nullptr) const;
};

struct PlayContext 
{
    Config& config;
    Snake& snake;
    Score& score;
    sf::Sound& sfxEat;
    std::function<void()> flashSnake;
};