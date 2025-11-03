#pragma once
#include "states/IGameState.h"
#include "core/Config.h"
#include "core/Input.h"
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
#include "render/GridBatch.h"
#include "render/ItemsRenderer.h"
#include "render/DebugOverlay.h"
#include "render/BackgroundRenderer.h"
#include "render/GateRenderer.h"
#include "render/RendererRegistry.h"
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

    bool showPerf_ = false;
    render::RendererRegistry rr_;

    render::GridBatch gridBatch_;
    // metrics ms draw()
    float tBgMs_ = 0.f, tGridMs_ = 0.f, tPortalsMs_ = 0.f, tSnakeMs_ = 0.f, tUIMs_ = 0.f;

    // screen-space overlays
    void drawConfuseOverlay_(sf::RenderTarget& rt, const sf::View& worldView);

    std::vector<InputAction> actionQueue_;
    void applyActions_();

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

    // EPH: cache of generated obstacles, and toggle state
    std::vector<sf::Vector2i> ephObstacles_;
    std::vector<Vec2i>        ephCells_;
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
    void  clearGate() { gateUnlocked_ = false; gateCell_ = Vec2i(-1, -1); }
    void  startLevel(int idx); // reinit level
    int   levelTarget() const { return levelTargets_[std::clamp(levelIndex_, 1, 3) - 1]; }

    // texture sprites
    sf::Sprite sprGround_, sprWall_, sprObs_;
    sf::Sprite sprApple1_, sprApple2_, sprApple3_;
    sf::Sprite sprHead_, sprBody_, sprTail_;
    sf::Sprite sprBodyCorner_[4];
    sf::Sprite sprPowerBomb_, sprPowerMush_;
    sf::Sprite sprExpl_;

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
    inline void fitSpriteToCell(sf::Sprite& s, const sf::Texture& tx) const
    {
        const auto sz = tx.getSize();
        s.setOrigin(sf::Vector2f(sz.x * 0.5f, sz.y * 0.5f));
        s.setScale(cfg_.cellPx / static_cast<float>(sz.x),
            cfg_.cellPx / static_cast<float>(sz.y));
    }

    // occupancy grid 
    std::vector<uint8_t> snakeOcc_;
    bool snakeOccDirty_ = true;
    inline int occIndex_(int x, int y) const { return y * cfg_.gridWidth + x; }
    void rebuildSnakeOcc_();
    inline bool isSnakeOcc_(int x, int y) const 
    {
        if (x < 0 || y < 0 || x >= cfg_.gridWidth || y >= cfg_.gridHeight) return false;
        if (snakeOcc_.empty()) return false;
        return snakeOcc_[occIndex_(x, y)] != 0;
    
    }

    // Scratch buffer
    std::vector<sf::Vector2f> expPosScratch_;
    std::vector<float>        expTScratch_;

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
    float chromAbAmountPx_ = 30.0f;
    sf::Vector2f chromResCached_{ -1.f, -1.f };
    float chromAmountCached_{ -1.f };

    // ca helpers
    void ensureWorldRT_();
};