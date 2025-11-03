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
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
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
    ~PlayState();

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

    sf::Text scoreRGB_;
    float scoreRGBElapsed_ = 0.f;

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

    // texture sprites
    sf::Sprite sprGround_, sprWall_, sprObs_;
    sf::Sprite sprApple1_, sprApple2_, sprApple3_;
    sf::Sprite sprHead_, sprBody_, sprTail_;
    sf::Sprite sprBodyCorner_[4];
    sf::Sprite sprPowerBomb_, sprPowerMush_;
    sf::Sprite sprExpl_;
    // tile ground sprites 
    std::vector<sf::Sprite> groundTiles_;
    void rebuildGroundTiles();

    sf::VertexArray groundVA_;
    void buildGroundTilemap();

    sf::Shader groundDesat_;
    bool groundDesatReady_ = false;
    float groundSaturation_ = 0.55f;

    // texture helpers
    inline sf::Vector2f cellCenter(int cx, int cy) const 
    {
        const float cs = static_cast<float>(cfg_.cellPx);
        return { cx * cs + cs * 0.5f, cy * cs + cs * 0.5f };
    }
    inline void fitSpriteToCell(sf::Sprite & s, const sf::Texture & tx) const 
    {
        const auto sz = tx.getSize();
        s.setOrigin(sf::Vector2f(sz.x * 0.5f, sz.y * 0.5f));
        s.setScale(cfg_.cellPx / static_cast<float>(sz.x),
        cfg_.cellPx / static_cast<float>(sz.y));
    }

    // vfx explosion
    struct ExplFx { sf::Vector2f pos; float t; };
    std::vector<ExplFx> explFx_;

    // wallbreaker banner
    sf::Text breakerBanner_;
    float breakerBannerT_ = 0.f;
    float breakerBannerElapsed_ = 0.f;
    void showBreakerBanner();

    // turbosnake banner
    sf::Text turboBanner_;
    float turboBannerT_ = 0.f;
    float turboBannerElapsed_ = 0.f;
    void showTurboBanner();

    // poison/confuse banner
    sf::Text poisonBanner_;
    float poisonBannerT_ = 0.f;
    float poisonBannerElapsed_ = 0.f;
    void showPoisonBanner();

    int portalIndexAt(int gx, int gy) const;
    std::vector<float> portalAnim_;
    float portalFrameTime_ = 0.06f;
    float portalCooldown_ = 0.0f;
    void updatePortals(float dt);
    void drawPortals(sf::RenderTarget& rt);

    // Emissive glow
    sf::Shader portalGlow_;
    bool portalGlowReady_ = false;
    float portalGlowStrength_ = 0.75f;
    float portalHaloScale_ = 1.30f;
    float portalGlowPulseHz_ = 1.6f;

    // confuse overlay
    sf::Shader confuseOverlay_;
    bool confuseOverlayReady_ = false;
    float confusePulseSpeed_ = 14.0f;
    bool confuseWasActive_ = false;

    // chromatic aberration
    sf::RenderTexture worldRT_;
    bool worldRTReady_ = false;
    sf::Shader  chromAb_;
    bool chromAbReady_ = false;
    float chromAbAmountPx_ = 3.0f;

    // ca helpers
    void ensureWorldRT_();
    void drawWorldLayer_(sf::RenderTarget& rt);
};

struct PlayContext 
{
    Config& config;
    Snake& snake;
    Score& score;
    sf::Sound& sfxEat;
    std::function<void()> flashSnake;
};