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
#include <array>
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

    std::vector<PowerUp> powerups_;
    float puSpawnCooldown_{ 5.f };
    float puSpawnMin_{ 6.f }, puSpawnMax_{ 12.f };
    bool isCellFree(int x, int y) const;
    void spawnBreakerPU();

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

    sf::Sound               sfxEat_, sfxDeath_, sfxBonus_, sfxBreak_, sfxPortal_;             
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

    // HUD Breaker
    sf::Text brText_;
    sf::RectangleShape brBack_;
    sf::RectangleShape brFill_;
    sf::Vector2f brPos_{ 16.f, 16.f };
    sf::Vector2f brSize_{ 160.f, 12.f };

    float brUiMaxSec_{ 3.0f };
    float brUiUpdateThrottle_{ 0.0f };

    // Level progression
    int   levelIndex_{ 1 };
    int   scoreAtLevelStart_{ 0 }; // init mark to enter on level
    std::array<int, 3> levelTargets_{ {50,100,250} };
    // Level Gate
    bool  gateUnlocked_{ false };
    Vec2i gateCell_{ -1,-1 };
    sf::RectangleShape gateViz_;
    float gatePulse_{ 0.f };

    // Stage overlay (STAGE 1/2/3)
    sf::Text stageText_;
    float stageTimer_{ 0.f };
    void  showStageBanner();

    // helpers
    void  maybeUnlockGate(); // open gate check
    bool  isBorderNonCorner(int x, int y) const;
    void  unlockGate();
    void  clearGate();
    void  startLevel(int idx); // reinit level
    int   levelTarget() const { return levelTargets_[std::clamp(levelIndex_, 1, 3) - 1]; }
};

struct PlayContext 
{
    Config& config;
    Snake& snake;
    Score& score;
    sf::Sound& sfxEat;
    std::function<void()> flashSnake;
};