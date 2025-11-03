#include "states/PlayState.h"
#include "systems/Spawner.h"
#include "systems/Collision.h"
#include "render/SnakeRenderer.h"
#include "render/PortalsRenderer.h"
#include "render/UIRenderer.h"
#include "render/GridBatch.h"
#include "render/ItemsRenderer.h"
#include "render/DebugOverlay.h"
#include "render/BackgroundRenderer.h"
#include "render/GateRenderer.h"
#include "render/RendererRegistry.h"
#include "states/PauseState.h"
#include "states/GameOverState.h"
#include "states/VictoryState.h"
#include "states/PlayContext.h"
#include "entities/Powerup.h"
#include "core/StateMachine.h"
#include "world/ProcGen.h"
#include <SFML/Graphics.hpp>
#include "core/Perf.h"
#include <functional>
#include <algorithm>
#include <string>
#include <cstdio> 
#include <cmath>

PlayState::PlayState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
    : sm_(sm), win_(win), cfg_(cfg), res_(res),
    level_(Level::loadFromFile("data/levels/level1.txt", cfg.gridWidth, cfg.gridHeight)),
    snake_({ cfg.gridWidth / 2, cfg.gridHeight / 2 }),
    hud_(res.font())
{
    sfxEat_.setBuffer(res_.sfxEat());
    sfxDeath_.setBuffer(res_.sfxDeath());
    sfxBonus_.setBuffer(res_.sfxBonus());
    sfxBreak_.setBuffer(res_.sfxBreak());
    sfxPortal_.setBuffer(res_.sfxPortal());

    // texture sprites linking
    sprGround_.setTexture(res_.txGround());
    sprWall_.setTexture(res_.txWall());
    sprObs_.setTexture(res_.txObstacle());
    sprApple1_.setTexture(res_.txApple1());
    sprApple2_.setTexture(res_.txApple2());
    sprApple3_.setTexture(res_.txApple3());
    sprPowerBomb_.setTexture(res_.txPowerBomb());
    sprPowerMush_.setTexture(res_.txPowerMush());
    sprExpl_.setTexture(res_.txExplosion());
    sprHead_.setTexture(res_.txSnakeHead());
    sprBody_.setTexture(res_.txSnakeBody());
    sprTail_.setTexture(res_.txSnakeTail());
    sprBodyCorner_[0].setTexture(res_.txBodyC1());
    sprBodyCorner_[1].setTexture(res_.txBodyC2());
    sprBodyCorner_[2].setTexture(res_.txBodyC3());
    sprBodyCorner_[3].setTexture(res_.txBodyC4());
    
    if (auto tex = sprExpl_.getTexture()) 
    {
        const auto sz = tex->getSize();
        sprExpl_.setOrigin(sz.x * 0.5f, sz.y * 0.5f);
    }

    if (auto* mg = res_.music()) mg->setPitch(1.0f);
    confuseWasActive_ = false;
}

PlayState::~PlayState()
{
    if (auto* mg = res_.music()) mg->setPitch(1.0f);
}

void PlayState::onEnter() 
{
    win_.setView(win_.getDefaultView());
    effects_.reset(); // clear effects
    appleKind_ = AppleKind::Normal;
    appleTTL_ = 0.f;
    if (initialized_) 
    {
        return;
    }
    level_.clear();
    level_.buildBorders();
    res_.ensureSessionLoop();

    // center or snake init spawn cell
    sf::Vector2i start{ level_.cols() / 2, level_.rows() / 2 };

    // difficult params
    ProcGenParams g;
    switch (static_cast<int>(cfg_.difficulty))
    {
        case 1: g.maxPlacements = 12; g.maxSingles = 6;  g.minReachable = 0.75f; break;
        case 2: g.maxPlacements = 16; g.maxSingles = 8;  g.minReachable = 0.70f; break;
        case 3: g.maxPlacements = 20; g.maxSingles = 10; g.minReachable = 0.65f; break;
        case 4: g.maxPlacements = 24; g.maxSingles = 12; g.minReachable = 0.60f; break;
        case 5: g.maxPlacements = 28; g.maxSingles = 14; g.minReachable = 0.55f; break;
        default: break;
    }

    static thread_local std::mt19937 rng{ std::random_device{}() };
    auto obstacles = ProcGen::generate(level_.cols(), level_.rows(), start, g, rng);
    level_.applyObstacles(obstacles);

    //Breaker
    brText_.setCharacterSize(std::max(14, int(cfg_.cellPx * 0.6f)));
    brText_.setFillColor(sf::Color::White);
    brText_.setOutlineColor(sf::Color(0, 0, 0, 200));
    brText_.setOutlineThickness(2.f);
    brText_.setPosition(brPos_.x, brPos_.y - 20.f);
    brBack_.setSize(brSize_);
    brBack_.setPosition(brPos_);
    brBack_.setFillColor(sf::Color(0, 0, 0, 120));
    brBack_.setOutlineThickness(1.f);
    brBack_.setOutlineColor(sf::Color(255, 255, 255, 180));
    brFill_.setSize({ 0.f, brSize_.y });
    brFill_.setPosition(brPos_);
    brFill_.setFillColor(sf::Color(50, 205, 50));

    // prepare sprite scale for grid
    fitSpriteToCell(sprWall_, *sprWall_.getTexture());
    fitSpriteToCell(sprObs_, *sprObs_.getTexture());
    fitSpriteToCell(sprApple1_, *sprApple1_.getTexture());
    fitSpriteToCell(sprApple2_, *sprApple2_.getTexture());
    fitSpriteToCell(sprApple3_, *sprApple3_.getTexture());
    fitSpriteToCell(sprPowerBomb_, *sprPowerBomb_.getTexture());
    fitSpriteToCell(sprPowerMush_, *sprPowerMush_.getTexture());
    fitSpriteToCell(sprExpl_, *sprExpl_.getTexture());
    fitSpriteToCell(sprHead_, *sprHead_.getTexture());
    fitSpriteToCell(sprBody_, *sprBody_.getTexture());
    fitSpriteToCell(sprTail_, *sprTail_.getTexture());
    for (int i = 0; i < 4; ++i)
        fitSpriteToCell(sprBodyCorner_[i], *sprBodyCorner_[i].getTexture());

    // EPH
    ephObstacles_ = obstacles;
    ephVisible_ = true;
    ephTimer_ = 0.f;
    level_.applyObstacles(ephObstacles_);

    buildGroundTilemap();
    gridBatch_.build(level_, cfg_.cellPx, res_.txWall(), res_.txObstacle());

    // SHADERS block
    // ground desat shader init
    {
        const char* frag = R"(
        uniform sampler2D texture;
        uniform float u_saturation;
        void main()
        {
            vec4 c = texture2D(texture, gl_TexCoord[0].xy);
            float gray = dot(c.rgb, vec3(0.299, 0.587, 0.114));
            vec3 desat = mix(vec3(gray), c.rgb, u_saturation);
            gl_FragColor = vec4(desat, c.a);
        })";
        groundDesatReady_ = groundDesat_.loadFromMemory(frag, sf::Shader::Fragment);
        if (groundDesatReady_) 
        {
            groundDesat_.setUniform("u_saturation", groundSaturation_);
            groundDesat_.setUniform("texture", sf::Shader::CurrentTexture);
        }
    }

    // confuse overlay shader
    {
        const char* frag = R"(
        uniform float u_time;
        uniform vec2  u_res;
        void main() 
        {
            vec2 uv = gl_FragCoord.xy / u_res;
            float r = 0.5 + 0.5 * sin(u_time + uv.x * 6.2831853);
            float g = 0.5 + 0.5 * sin(u_time + 2.0943951 + uv.y * 6.2831853);
            float b = 0.5 + 0.5 * sin(u_time + 4.1887902 + (uv.x + uv.y) * 3.1415926);

            gl_FragColor = vec4(r, g, b, 0.18);
        })";
        confuseOverlayReady_ = confuseOverlay_.loadFromMemory(frag, sf::Shader::Fragment);
    }

    // chromatic aberration shader
    {
        const char* frag = R"(
        uniform sampler2D texture;
        uniform vec2  u_res;
        uniform float u_amount;
        uniform float u_time;

        void main() 
        {
            vec2 uv = gl_TexCoord[0].xy;

            vec2 center = vec2(0.5, 0.5);
            vec2 d = uv - center;
            float r = length(d) + 1e-6;

            float amtUV = (u_amount / u_res.x) * (0.95 + 0.15 * sin(u_time * 6.2831853));
            vec2 dir = d / r;

            vec2 off = dir * amtUV * r * 2.0;

            float rr = texture2D(texture, uv + off).r;
            float gg = texture2D(texture, uv).g;
            float bb = texture2D(texture, uv - off).b;

            vec4 src = texture2D(texture, uv);
            gl_FragColor = vec4(rr, gg, bb, src.a);
        })";
        chromAbReady_ = chromAb_.loadFromMemory(frag, sf::Shader::Fragment);
        if (chromAbReady_) 
        {
            chromAb_.setUniform("texture", sf::Shader::CurrentTexture);
        }
    }

    // --- portal emissive glow shader init ---
    {
        const char* frag = R"(
        uniform sampler2D texture;
        uniform vec3  u_tint;
        uniform float u_strength;
        uniform float u_time;
        uniform vec2  u_texel;   // 1/width, 1/height текстуры

        void main() 
        {
            vec2 uv = gl_TexCoord[0].xy;
            vec4 t  = texture2D(texture, uv);
            float a = t.a;

            float core = smoothstep(0.0, 0.7, a);

            vec2 o = u_texel * 1.5;
            float n = 0.0;
            n += texture2D(texture, uv + vec2( o.x, 0.0)).a;
            n += texture2D(texture, uv + vec2(-o.x, 0.0)).a;
            n += texture2D(texture, uv + vec2(0.0,  o.y)).a;
            n += texture2D(texture, uv + vec2(0.0, -o.y)).a;
            float edge = n * 0.25;

            float pulse = 0.65 + 0.35 * sin(u_time);

            float g = (core * 0.85 + edge * 0.65) * pulse * u_strength;

            vec3 glow = u_tint * g;
            gl_FragColor = vec4(glow, g);
        })";
        portalGlowReady_ = portalGlow_.loadFromMemory(frag, sf::Shader::Fragment);
        if (portalGlowReady_) 
        {
            portalGlow_.setUniform("texture", sf::Shader::CurrentTexture);
        }
    }

    spawnPortals(1);
    spawnApple();
    showStageBanner();
    initialized_ = true;

    scoreAtLevelStart_ = score_.value();
    gateUnlocked_ = false;
    gateCell_ = Vec2i(-1, -1);

    scoreRGB_.setFont(res_.font());
    scoreRGB_.setCharacterSize(28);
    scoreRGB_.setOutlineThickness(2.f);
    scoreRGB_.setFillColor(sf::Color::White);
    scoreRGB_.setOutlineColor(sf::Color::Black);
}

bool PlayState::isBorderNonCorner(int x, int y) const
{
    const int W = level_.cols();
    const int H = level_.rows();
    const bool border = (x == 0 || y == 0 || x == W - 1 || y == H - 1);
    const bool corner = ((x == 0 || x == W - 1) && (y == 0 || y == H - 1));
    return border && !corner;
}

void PlayState::unlockGate()
{
    const int W = level_.cols();
    const int H = level_.rows();

    // trying seeking borders (non corners)
    for (int guard = 0; guard < 100; ++guard) 
    {
        int side = std::uniform_int_distribution<int>(0, 3)(rng_); // 0:top,1:bottom,2:left,3:right
        Vec2i c;
        if (side == 0) { c = Vec2i(std::uniform_int_distribution<int>(1, W - 2)(rng_), 0); }
        else if (side == 1) { c = Vec2i(std::uniform_int_distribution<int>(1, W - 2)(rng_), H - 1); }
        else if (side == 2) { c = Vec2i(0, std::uniform_int_distribution<int>(1, H - 2)(rng_)); }
        else { c = Vec2i(W - 1, std::uniform_int_distribution<int>(1, H - 2)(rng_)); }

        if (!isBorderNonCorner(c.x, c.y)) continue;
        if (!level_.grid().isObstacle(c.x, c.y)) continue;

        // open gates
        gateCell_ = c;
        gateUnlocked_ = true;
        level_.grid().destroyObstacle(c.x, c.y);
        gridBatch_.markDirty();

        gateViz_.setSize(sf::Vector2f((float)cfg_.cellPx, (float)cfg_.cellPx));
        gateViz_.setOrigin(gateViz_.getSize() * 0.5f);
        gateViz_.setPosition(c.x * cfg_.cellPx + cfg_.cellPx * 0.5f,
                             c.y * cfg_.cellPx + cfg_.cellPx * 0.5f);
        gateViz_.setFillColor(sf::Color(120, 220, 120, 220));
        gateViz_.setOutlineThickness(2.f);
        gateViz_.setOutlineColor(sf::Color::Black);
        gatePulse_ = 0.f;
        break;
    }
}

void PlayState::maybeUnlockGate()
{
    if (gateUnlocked_) return;
    const int gained = score_.value() - scoreAtLevelStart_;
    if (gained >= levelTarget()) 
    {
        unlockGate();
    }
}

void PlayState::startLevel(int idx)
{
    levelIndex_ = std::clamp(idx, 1, 3);
    cfg_.difficulty = static_cast<Difficulty>(std::clamp(levelIndex_, 1, 5));

    // replace base score checkpoint at start level
    scoreAtLevelStart_ = score_.value();

    // full reset game state
    initialized_ = false;
    level_.clear();
    clearGate();  // close gate
    portals_.clear();
    powerups_.clear();
    apple_.reset();
    effects_.reset();
    timeAcc_ = 0.f;
    startDelay_ = 0.35f; // start delay

    // snake reset
    const int cx = level_.grid().w() / 2;
    const int cy = level_.grid().h() / 2;
    snake_ = Snake({ cx, cy });

    win_.setView(win_.getDefaultView());

    onEnter(); // generate new level/apples/portals
}

void PlayState::spawnPortals(int pairs)
{
    portals_.clear();
    portalCooldown_ = 0.f;

    static thread_local std::mt19937 rng{ std::random_device{}() };
    for (int i = 0; i < pairs; ++i)
    {
        Vec2i a = Spawner::randomFreeCell(level_.grid(), snake_);
        Vec2i b = Spawner::randomFreeCell(level_.grid(), snake_);
        int guard = 0;
        auto eq = [](const Vec2i& u, const Vec2i& v) { return u.x == v.x && u.y == v.y; };
        while ((eq(a, b) || level_.isBlocked(a.x, a.y) || level_.isBlocked(b.x, b.y)) && guard++ < 500)
        {
            a = Spawner::randomFreeCell(level_.grid(), snake_);
            b = Spawner::randomFreeCell(level_.grid(), snake_);
        }

        PortalPair p; p.a = a; p.b = b;
        static const sf::Color colors[] = { {120,200,255},{255,160,80},{180,255,120},{220,120,255} };
        p.color = colors[i % (int)std::size(colors)];
        portals_.push_back(p);
    }
    portalAnim_.assign(portals_.size(), 0.f);
}

bool PlayState::isPortalCell(const Vec2i& c, size_t* outPairIdx, bool* isA) const
{
    for (size_t i = 0; i < portals_.size(); ++i) 
    {
        if (portals_[i].a.x == c.x && portals_[i].a.y == c.y) { if (outPairIdx) *outPairIdx = i; if (isA) *isA = true;  return true; }
        if (portals_[i].b.x == c.x && portals_[i].b.y == c.y) { if (outPairIdx) *outPairIdx = i; if (isA) *isA = false; return true; }
    }
    return false;
}

void PlayState::showStageBanner()
{
    stageTimer_ = 1.0f;
    stageText_.setFont(res_.font());
    stageText_.setString(L"STAGE " + std::to_wstring(levelIndex_));
    stageText_.setCharacterSize(72);
    stageText_.setFillColor(sf::Color::White);
    stageText_.setOutlineThickness(2.f);
    stageText_.setOutlineColor(sf::Color::Black);
}

void PlayState::handleEvent(const sf::Event& e)
{
    // game input
    if (e.type == sf::Event::KeyPressed)
    {
        auto key = e.key.code;

        if (effects_.inverted()) 
        {
            // control inverting
            if (key == sf::Keyboard::W)          key = sf::Keyboard::S;
            else if (key == sf::Keyboard::S)     key = sf::Keyboard::W;
            else if (key == sf::Keyboard::A)     key = sf::Keyboard::D;
            else if (key == sf::Keyboard::D)     key = sf::Keyboard::A;
            else if (key == sf::Keyboard::Up)    key = sf::Keyboard::Down;
            else if (key == sf::Keyboard::Down)  key = sf::Keyboard::Up;
            else if (key == sf::Keyboard::Left)  key = sf::Keyboard::Right;
            else if (key == sf::Keyboard::Right) key = sf::Keyboard::Left;
        }

        if (key == sf::Keyboard::W || key == sf::Keyboard::Up)
            snake_.setDirection(Direction::Up);
        if (key == sf::Keyboard::S || key == sf::Keyboard::Down)
            snake_.setDirection(Direction::Down);
        if (key == sf::Keyboard::A || key == sf::Keyboard::Left)
            snake_.setDirection(Direction::Left);
        if (key == sf::Keyboard::D || key == sf::Keyboard::Right)
            snake_.setDirection(Direction::Right);
        if (key == sf::Keyboard::P) 
        {
            sm_.push(std::make_unique<PauseState>(sm_, res_));
        }
    }
    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
        case sf::Keyboard::F3:  showPerf_ = !showPerf_; break;
        case sf::Keyboard::F4:  rr_.toggleBackground(); break;
        case sf::Keyboard::F5:  rr_.toggleGrid();       break;
        case sf::Keyboard::F6:  rr_.togglePortals();    break;
        case sf::Keyboard::F7:  rr_.toggleItems();      break;
        case sf::Keyboard::F8:  rr_.toggleSnake();      break;
        case sf::Keyboard::F9:  rr_.toggleGate();       break;
        case sf::Keyboard::F10: rr_.toggleConfuseFx();  break;
        case sf::Keyboard::F11: rr_.toggleUI();         break;
        default: break;
        }
    }
}

void PlayState::update(float dt) 
{
    effects_.update(dt);
    shake_.update(dt);
    snake_.update(dt);
    snake_.tickBreaker(dt);
    maybeUnlockGate();

    puSpawnCooldown_ -= dt;
    if (puSpawnCooldown_ <= 0.f) 
    {
        spawnBreakerPU();
        std::uniform_real_distribution<float> dist(puSpawnMin_, puSpawnMax_);
        static std::mt19937 rng{ std::random_device{}() };
        puSpawnCooldown_ = dist(rng);
    }

    // Stage overlay timer
    if (stageTimer_ > 0.f) 
    {
       stageTimer_ -= dt;
       if (stageTimer_ < 0.f) stageTimer_ = 0.f;
    }

    for (auto& p : powerups_) p.ttl -= dt;
    powerups_.erase(std::remove_if(powerups_.begin(), powerups_.end(),
        [](const PowerUp& p) { return p.ttl <= 0.f; }), powerups_.end());

    confuseHueT_ += dt;
    if (confuseVisT_ > 0.f) confuseVisT_ -= dt;

    if (startDelay_ > 0.f) { startDelay_ -= dt; return; }

    if (apple_ && appleKind_ != AppleKind::Normal && appleTTL_ > 0.f) 
    {
        appleTTL_ -= dt;
        if (appleTTL_ <= 0.f) 
        {
            apple_.reset();
            spawnApple();
        }
    }

    // confuse music uptempo
    {
        const bool activeNow = (confuseVisT_ > 0.0001f);

        if (activeNow != confuseWasActive_) 
        {
            if (auto* mg = res_.music())
            {
                mg->setPitch(activeNow ? 1.2f : 1.0f);
            }
            confuseWasActive_ = activeNow;
        }
    }

    const float dtEffective = dt * effects_.speedMul();

    timeAcc_ += dtEffective;
    portalPulseT_ += dt;

    const float left = snake_.breakerTimeLeft();
    if (left > 0.f) 
    {
        // progressbar
        const float frac = std::clamp(left / brUiMaxSec_, 0.f, 1.f);
        brFill_.setSize({ brSize_.x * frac, brSize_.y });

        brUiUpdateThrottle_ -= dt;
        if (brUiUpdateThrottle_ <= 0.f) 
        {
            brUiUpdateThrottle_ = 0.1f;
            char buf[32];
            std::snprintf(buf, sizeof(buf), "Breaker: %.1fs", left);
            brText_.setString(buf);
        }
        // flick
        if (left <= 1.0f) 
        {
            float pulse = 0.5f * (1.f + std::sin(2.f * 3.1415926f * 5.f * left));
            sf::Uint8 a = static_cast<sf::Uint8>(120 + 135 * pulse);
            auto fill = brFill_.getFillColor();
            fill.a = a;
            brFill_.setFillColor(fill);
            auto txt = brText_.getFillColor();
            txt.a = a;
            brText_.setFillColor(txt);
        }
        else 
        {
            auto fill = brFill_.getFillColor();
            fill.a = 255;
            brFill_.setFillColor(fill);
            auto txt = brText_.getFillColor();
            txt.a = 255;
            brText_.setFillColor(txt);
        }
    }
    else 
    {
        brFill_.setSize({ 0.f, brSize_.y });
        brText_.setString("");
    }

    const float step = cfg_.paramsFor(cfg_.difficulty).stepSec;
    while (timeAcc_ >= step) 
    {
        timeAcc_ -= step;
        snake_.step();

        // portals
        {
            if (portalCooldown_ <= 0.f) 
            {
                size_t pairIdx = 0;
                bool isA = false;
                const Vec2i head = snake_.head();

                if (isPortalCell(head, &pairIdx, &isA)) 
                {
                    const Vec2i dst = isA ? portals_[pairIdx].b : portals_[pairIdx].a;

                    auto snakeOccupies = [&](int x, int y) 
                        {
                        for (const auto& c : snake_.body())
                            if (c.x == x && c.y == y) return true;
                        return false;
                        };

                    if (level_.isBlocked(dst.x, dst.y) || snakeOccupies(dst.x, dst.y)) 
                    {
                        die();
                        return;
                    }

                    snake_.teleportHead(dst);
                    res_.playSfx(sfxPortal_, 100.f);
                    portalCooldown_ = 0.15f;
                }
            }
        }

        if (gateUnlocked_ && snake_.head().x == gateCell_.x && snake_.head().y == gateCell_.y)
            {
            sm_.push(std::make_unique<VictoryState>(
                sm_, win_, cfg_, res_, levelIndex_, score_.value(),
                [this]
                {
                if (levelIndex_ < 3) startLevel(levelIndex_ + 1);
                else 
                {
                    sm_.push(std::make_unique<GameOverState>(sm_, win_, cfg_, res_, score_.value()));
                    res_.switchToGameOver();
                }
                }));
            return;
            }

        const Vec2i h = snake_.head();
        if (level_.grid().isObstacle(h.x, h.y) && snake_.canBreakObstacles()) 
        {
            if (!level_.grid().isBorder(h.x, h.y)) 
            {
                level_.grid().destroyObstacle(h.x, h.y);
                gridBatch_.markDirty();
                res_.playSfx(sfxBreak_, 100.f);
                explFx_.push_back({ cellCenter(h.x, h.y), 0.25f });
                shake_.start(0.12f, 2.0f);
            }
        }

        // death
        if (Collision::headHitsWall(level_.grid(), snake_) || snake_.bitesItself()) 
        {
            die();
            return;
        }
        // apple eating
        if (apple_ && snake_.head() == apple_->cell())
        {
            PlayContext ctx{ cfg_, snake_, score_, sfxEat_, [this] { this->flashSnake(); } };
            apple_->onEaten(ctx);

            const int P = cfg_.paramsFor(cfg_.difficulty).pointsPerApple;

            switch (appleKind_)
            {
            case AppleKind::Bonus:
            {
                const int extra = (int)std::round((cfg_.apple.bonusScoreMul - 1.0f) * P);
                score_.add(extra);
                break;
            }
            case AppleKind::Poison:
                effects_.applySpeed(cfg_.apple.poisonSpeedMul, cfg_.apple.poisonDuration);
                showTurboBanner();
                break;
            case AppleKind::Confuse:
                effects_.applyInvert(cfg_.apple.confuseDuration);
                confuseVisT_ = cfg_.apple.confuseDuration;
                showPoisonBanner();
                break;
            default:
                break;
            }
            spawnApple();
            shake_.start(0.12f, 2.0f);
        }

        const auto head = snake_.head();

        for (size_t i = 0; i < powerups_.size(); ++i) 
        {
            const auto& p = powerups_[i];
            if (p.cellX == head.x && p.cellY == head.y) 
            {
                if (p.kind == PowerUpKind::Breaker) 
                {
                    snake_.enableBreaker(3.0f);
                    brUiMaxSec_ = 3.0f;
                    res_.playSfx(sfxBonus_, 100.f);
                }
                powerups_.erase(powerups_.begin() + i);
                showBreakerBanner();
                break;
            }
        }
    }

    // EPH: toggle visibility of temporary obstacles
    if (cfg_.ephemeralObstacles) 
    {
        ephTimer_ += dt;
        const float period = ephVisible_ ? cfg_.ephemeralOnSec : cfg_.ephemeralOffSec;

        if (ephTimer_ >= period) 
        {
            ephTimer_ = 0.f;

            if (ephVisible_) 
            {
                ephApplyHidden();
                ephVisible_ = false;
            }
            else 
            {
                ephCells_.clear();

                const int N = 6;
                for (int i = 0; i < N; ++i) 
                {
                    auto c = Spawner::randomFreeCell(level_.grid(), snake_);
                    if (std::find(ephCells_.begin(), ephCells_.end(), c) == ephCells_.end()) 
                    {
                        ephCells_.push_back(c);
                    }
                }
                ephApplyVisible();
                ephVisible_ = true;
            }
        }
    }
    // vfx explosion
    for (auto it = explFx_.begin(); it != explFx_.end(); ) 
    {
        it->t -= dt;
        if (it->t <= 0.f) it = explFx_.erase(it);
        else ++it;
    }

    // wallbreaker banner timers
    if (breakerBannerT_ > 0.f) 
    {
        breakerBannerT_ -= dt;
        if (breakerBannerT_ < 0.f) breakerBannerT_ = 0.f;
        breakerBannerElapsed_ += dt; // rgb
    }

    // turbosnake banner timers
    if (turboBannerT_ > 0.f) 
    {
        turboBannerT_ -= dt;
        if (turboBannerT_ < 0.f) turboBannerT_ = 0.f;
        turboBannerElapsed_ += dt; // rgb
    }

    // poison banner timers
    if (poisonBannerT_ > 0.f)
    {
        poisonBannerT_ -= dt;
        if (poisonBannerT_ < 0.f) poisonBannerT_ = 0.f;
        poisonBannerElapsed_ += dt; // rgb
    }
    scoreRGBElapsed_ += dt;
    updatePortals(dt);

}

bool PlayState::isCellFree(int x, int y) const
{
    if (level_.isBlocked(x, y)) return false;

    if (apple_ && apple_->cell().x == x && apple_->cell().y == y) return false;

    for (const auto& c : snake_.body())
        if (c.x == x && c.y == y) return false;

    size_t idx; bool isA; if (isPortalCell({x,y}, &idx, &isA)) return false;

    return true;
}

void PlayState::ensureWorldRT_()
{
    const unsigned W = static_cast<unsigned>(cfg_.gridWidth * cfg_.cellPx);
    const unsigned H = static_cast<unsigned>(cfg_.gridHeight * cfg_.cellPx);
    if (!worldRTReady_ || worldRT_.getSize().x != W || worldRT_.getSize().y != H) 
    {
        worldRTReady_ = worldRT_.create(W, H);
        worldRT_.setSmooth(true);
    }
}

void PlayState::spawnBreakerPU()
{
    static std::mt19937 rng{ std::random_device{}() };
    const int W = level_.grid().w();
    const int H = level_.grid().h();

    for (int i = 0; i < 40; ++i) 
    {
        int x = std::uniform_int_distribution<int>(0, W - 1)(rng);
        int y = std::uniform_int_distribution<int>(0, H - 1)(rng);
        if (!isCellFree(x, y)) continue;

        PowerUp pu;
        pu.cellX = x;
        pu.cellY = y;
        pu.kind = PowerUpKind::Breaker;
        pu.ttl = 12.f;
        powerups_.push_back(pu);
        break;
    }
}

void PlayState::buildGroundTilemap()
{
    const int W = level_.grid().w();
    const int H = level_.grid().h();
    const float CELL = static_cast<float>(cfg_.cellPx);

    groundVA_.setPrimitiveType(sf::Quads);
    groundVA_.resize(static_cast<std::size_t>(W) * static_cast<std::size_t>(H) * 4);

    const sf::Texture& tx = res_.txGround();
    const float tw = static_cast<float>(tx.getSize().x);
    const float th = static_cast<float>(tx.getSize().y);

    std::size_t v = 0;
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            const float px = x * CELL;
            const float py = y * CELL;

            groundVA_[v + 0].position = { px,         py };
            groundVA_[v + 1].position = { px + CELL,  py };
            groundVA_[v + 2].position = { px + CELL,  py + CELL };
            groundVA_[v + 3].position = { px,         py + CELL };
            const float u0 = 0.5f, v0 = 0.5f;
            const float u1 = tw - 0.5f, v1 = th - 0.5f;
            groundVA_[v + 0].texCoords = { u0, v0 };
            groundVA_[v + 1].texCoords = { u1, v0 };
            groundVA_[v + 2].texCoords = { u1, v1 };
            groundVA_[v + 3].texCoords = { u0, v1 };

            v += 4;
        }
    }
}

// wallbreaker banner
void PlayState::showBreakerBanner()
{
    breakerBanner_.setFont(res_.font());
    breakerBanner_.setString("WALLBREAKER");
    breakerBanner_.setCharacterSize(72);
    breakerBanner_.setOutlineThickness(4.f);
    breakerBanner_.setFillColor(sf::Color::White);
    breakerBanner_.setOutlineColor(sf::Color::Black);

    const auto b = breakerBanner_.getLocalBounds();
    breakerBanner_.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);

    breakerBannerT_ = 1.6f;
    breakerBannerElapsed_ = 0.f;
}

void PlayState::showTurboBanner()
{
    turboBanner_.setFont(res_.font());
    turboBanner_.setString("TURBOSNAKE");
    turboBanner_.setCharacterSize(72);
    turboBanner_.setOutlineThickness(4.f);
    turboBanner_.setFillColor(sf::Color::White);
    turboBanner_.setOutlineColor(sf::Color::Black);

    const auto b = turboBanner_.getLocalBounds();
    turboBanner_.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);

    turboBannerT_ = 1.6f;
    turboBannerElapsed_ = 0.f;
}

void PlayState::showPoisonBanner()
{
    poisonBanner_.setFont(res_.font());
    poisonBanner_.setString("YOU GOT THE POISON");
    poisonBanner_.setCharacterSize(72);
    poisonBanner_.setOutlineThickness(4.f);
    poisonBanner_.setFillColor(sf::Color::White);
    poisonBanner_.setOutlineColor(sf::Color::Black);

    const auto b = poisonBanner_.getLocalBounds();
    poisonBanner_.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);

    poisonBannerT_ = 1.6f;
    poisonBannerElapsed_ = 0.f;
}

int PlayState::portalIndexAt(int gx, int gy) const
{
    for (size_t i = 0; i < portals_.size(); ++i)
    {
        const auto& p = portals_[i];
        if ((p.a.x == gx && p.a.y == gy) || (p.b.x == gx && p.b.y == gy))
            return static_cast<int>(i);
    }
    return -1;
}

void PlayState::updatePortals(float dt)
{
    for (auto& t : portalAnim_) t += dt;

    if (portalCooldown_ > 0.f) 
    {
        portalCooldown_ -= dt;
        if (portalCooldown_ < 0.f) portalCooldown_ = 0.f;
    }
}

// EPH
void PlayState::ephApplyVisible()
{
    for (const auto& p : ephCells_) level_.setCell(p.x, p.y, Cell::Solid);
    gridBatch_.markDirty();
}

void PlayState::ephApplyHidden()
{
    for (const auto& p : ephCells_) level_.setCell(p.x, p.y, Cell::Empty);
    gridBatch_.markDirty();
}

void PlayState::draw(sf::RenderTarget& rt)
{
    const int W = cfg_.gridWidth * cfg_.cellPx;
    const int H = cfg_.gridHeight * cfg_.cellPx;
    sf::View worldView(sf::FloatRect(0.f, 0.f, static_cast<float>(W), static_cast<float>(H)));

    sf::View shaken = worldView;
    shaken.move(std::round(shake_.offsetX()), std::round(shake_.offsetY()));

    const bool kChromAb = (confuseVisT_ > 0.f) && chromAbReady_;
    sf::RenderTarget* world = &rt;

    if (kChromAb)
    {
        ensureWorldRT_();
        worldRT_.clear(sf::Color::Black);
        worldRT_.setView(shaken);
        world = &worldRT_;
    }

    // world pass
    world->setView(shaken);

    if (rr_.background)
    {
        perf::ScopeTimer _(tBgMs_);
        render::drawBackground(*world, groundVA_, res_.txGround(),
            groundDesatReady_ ? &groundDesat_ : nullptr);
    }

    if (rr_.grid)
    {
        perf::ScopeTimer _(tGridMs_);
        gridBatch_.rebuildIfDirty(level_, cfg_.cellPx, res_.txWall(), res_.txObstacle());
        gridBatch_.draw(*world);
    }

    if (rr_.portals)
    {
        perf::ScopeTimer _(tPortalsMs_);
        render::drawPortals(*world, cfg_, res_, portals_, portalAnim_,
            &portalGlow_, portalGlowReady_, portalGlowStrength_,
            portalHaloScale_, portalGlowPulseHz_);
    }

    if (rr_.items)
    {
        render::drawItems(*world, cfg_, apple_.get(),
            static_cast<render::AppleKind>(static_cast<int>(appleKind_)),
            appleTTL_, powerups_,
            sprApple1_, sprApple2_, sprApple3_, sprPowerBomb_, sprPowerMush_);
    }

    if (rr_.snake)
    {
        perf::ScopeTimer _(tSnakeMs_);
        std::array<sf::Sprite*, 4> corners = { &sprBodyCorner_[0], &sprBodyCorner_[1], &sprBodyCorner_[2], &sprBodyCorner_[3] };
        render::drawSnake(*world, snake_, cfg_.cellPx, sprHead_, sprBody_, sprTail_, corners);
    }

    if (rr_.gate)
    {
        render::drawGate(*world, gateUnlocked_, gateViz_, gatePulse_);
    }

    // chromatic aberration composite
    if (kChromAb)
    {
        worldRT_.display();
        sf::Sprite full(worldRT_.getTexture());
        chromAb_.setUniform("u_res", sf::Glsl::Vec2(static_cast<float>(W), static_cast<float>(H)));
        chromAb_.setUniform("u_time", confuseHueT_);
        chromAb_.setUniform("u_amount", chromAbAmountPx_);
        sf::RenderStates rsChrom; rsChrom.shader = &chromAb_;
        rt.setView(worldView);
        rt.draw(full, rsChrom);
    }

    // confuse world-space overlay
    if (rr_.confuseFx)
    {
        drawConfuseOverlay_(rt, worldView);
    }

    // UI
    if (rr_.ui)
    {
        perf::ScopeTimer _(tUIMs_);
        render::UIParams ui
        {
            res_, score_, effects_, snake_,
            stageText_, stageTimer_,
            scoreRGB_, scoreRGBElapsed_,
            brBack_, brFill_, brText_,
            breakerBanner_, breakerBannerT_, breakerBannerElapsed_,
            turboBanner_,   turboBannerT_,   turboBannerElapsed_,
            poisonBanner_,  poisonBannerT_,  poisonBannerElapsed_
        };
        render::drawUI(rt, ui);
    }

    // perf overlay
    if (showPerf_)
    {
        render::PerfData pd;
        pd.tBgMs = tBgMs_;
        pd.tGridMs = tGridMs_;
        pd.tPortalsMs = tPortalsMs_;
        pd.tSnakeMs = tSnakeMs_;
        pd.tUIMs = tUIMs_;
        pd.gridQuadsWall = gridBatch_.quadsWall();
        pd.gridQuadsObs = gridBatch_.quadsObs();
        pd.snakeSegments = static_cast<int>(snake_.body().size());
        render::drawPerfOverlay(rt, res_, pd);
    }
}

// helpers: explosion
void PlayState::drawExplosions_(sf::RenderTarget& world)
{
    constexpr float kLife = 0.25f;
    sf::RenderStates rs;
    rs.texture = sprExpl_.getTexture();
    rs.blendMode = sf::BlendAdd;

    for (const auto& fx : explFx_)
    {
        const float tNorm = 1.0f - std::clamp(fx.t / kLife, 0.0f, 1.0f);
        const float sc = 0.90f + 1.10f * tNorm;
        const sf::Uint8 A = static_cast<sf::Uint8>(255.0f * std::sqrt(std::max(0.0f, 1.0f - tNorm)));

        sf::Sprite s = sprExpl_;
        s.setPosition(fx.pos);
        s.setScale(sc, sc);
        s.setColor(sf::Color(255, 255, 255, A));
        world.draw(s, rs);
    }
}

// helpers: confuse overlay
void PlayState::drawConfuseOverlay_(sf::RenderTarget& rt, const sf::View& worldView)
{
    if (!(confuseVisT_ > 0.f && confuseOverlayReady_)) return;
    const int Wpx = cfg_.gridWidth * cfg_.cellPx;
    const int Hpx = cfg_.gridHeight * cfg_.cellPx;

    sf::RectangleShape cover({ (float)Wpx, (float)Hpx });
    cover.setPosition(0.f, 0.f);

    confuseOverlay_.setUniform("u_time", confuseHueT_ * confusePulseSpeed_);
    confuseOverlay_.setUniform("u_res", sf::Glsl::Vec2((float)Wpx, (float)Hpx));

    sf::RenderStates rs;
    rs.shader = &confuseOverlay_;
    rs.blendMode = sf::BlendAdd;

    auto prev = rt.getView();
    rt.setView(worldView);
    rt.draw(cover, rs);
    rt.setView(prev);
}

void PlayState::spawnApple()
{
    // basic spawn
    apple_ = Spawner::spawnNormal(level_.grid(), snake_);

    // avoid portals
    int guard = 0;
    if (apple_) 
    {
        while (isPortalCell(apple_->cell(), nullptr, nullptr) && guard++ < 64) 
        {
            apple_ = Spawner::spawnNormal(level_.grid(), snake_);
        }
    }

    // roll apple type by weights from Config
    auto rollKind = [&]() -> AppleKind
    {
        const int sum = cfg_.apple.wNormal + cfg_.apple.wBonus + cfg_.apple.wPoison + cfg_.apple.wConfuse;
        static thread_local std::mt19937 rng{ std::random_device{}() };
        std::uniform_int_distribution<int> dist(1, std::max(1, sum));
        int r = dist(rng);

        if ((r -= cfg_.apple.wNormal) <= 0) return AppleKind::Normal;
        if ((r -= cfg_.apple.wBonus) <= 0) return AppleKind::Bonus;
        if ((r -= cfg_.apple.wPoison) <= 0) return AppleKind::Poison;
        return AppleKind::Confuse;
    };

    appleKind_ = rollKind();
    // TTL for bonus
    switch (appleKind_)
    {
        case AppleKind::Normal:  appleTTL_ = 0.f;                   break;
        case AppleKind::Bonus:   appleTTL_ = cfg_.apple.bonusTTL;   break;
        case AppleKind::Poison:  appleTTL_ = cfg_.apple.poisonTTL;  break;
        case AppleKind::Confuse: appleTTL_ = cfg_.apple.confuseTTL; break;
    }
}

void PlayState::die()
{
    if (auto* mg = res_.music()) mg->setPitch(1.0f);
    confuseWasActive_ = false;
    res_.playSfx(sfxDeath_, 100.f);
    sm_.push(std::make_unique<GameOverState>(sm_, win_, cfg_, res_, score_.value()));
    res_.switchToGameOver();
}

void PlayState::flashSnake() 
{}