#include "states/PlayState.h"
#include "systems/Spawner.h"
#include "systems/Collision.h"
#include "states/PauseState.h"
#include "states/GameOverState.h"
#include "states/VictoryState.h"
#include "entities/Powerup.h"
#include "core/StateMachine.h"
#include "world/ProcGen.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <algorithm>
#include <cstdio> 
#include <cmath>

PlayState::PlayState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
    : sm_(sm), win_(win), cfg_(cfg), res_(res),
    level_(Level::loadFromFile("data/levels/level1.txt", cfg.gridWidth, cfg.gridHeight)),
    snake_({ cfg.gridWidth / 2, cfg.gridHeight / 2 }),
    hud_(res.font())
{
    res_.playSfx(sfxEat_);
    res_.playSfx(sfxDeath_);
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

    // EPH
    ephObstacles_ = obstacles;
    ephVisible_ = true;
    ephTimer_ = 0.f;
    level_.applyObstacles(ephObstacles_);

    spawnPortals(1);
    spawnApple();
    initialized_ = true;

    scoreAtLevelStart_ = score_.value();
    gateUnlocked_ = false;
    gateCell_ = Vec2i(-1, -1);
}

bool PlayState::isBorderNonCorner(int x, int y) const
{
    const int W = level_.cols();
    const int H = level_.rows();
    const bool border = (x == 0 || y == 0 || x == W - 1 || y == H - 1);
    const bool corner = ((x == 0 || x == W - 1) && (y == 0 || y == H - 1));
    return border && !corner;
}

void PlayState::clearGate()
{
    if (gateUnlocked_ && level_.grid().inside(gateCell_)) {}
    gateUnlocked_ = false;
    gateCell_ = Vec2i(-1, -1);
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
        level_.grid().destroyObstacle(c.x, c.y); // turn to Empty
        gateCell_ = c;
        gateUnlocked_ = true;

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
    static thread_local std::mt19937 rng{ std::random_device{}() };

    for (int i = 0; i < pairs; ++i)
    {
        // Spawner::randomFreeCell(const Grid&, const Snake&)
        Vec2i a = Spawner::randomFreeCell(level_.grid(), snake_);
        Vec2i b = Spawner::randomFreeCell(level_.grid(), snake_);
        int guard = 0;
        auto eq = [](const Vec2i& u, const Vec2i& v) { return u.x == v.x && u.y == v.y; };
        while ((eq(a, b) || level_.isBlocked(a.x, a.y) || level_.isBlocked(b.x, b.y)) && guard++ < 500)
        {
            a = Spawner::randomFreeCell(level_.grid(), snake_);
            b = Spawner::randomFreeCell(level_.grid(), snake_);
        }

        PortalPair p;
        p.a = a;
        p.b = b;

        static const sf::Color colors[] = { {120,200,255}, {255,160,80}, {180,255,120}, {220,120,255} };
        p.color = colors[i % (int)std::size(colors)];

        portals_.push_back(p);
    }
    portalLockCell_ = Vec2i(-9999, -9999);
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

        Vec2i next = snake_.head();
        switch (snake_.direction()) 
{
        case Direction::Up:    --next.y; break;
        case Direction::Down:  ++next.y; break;
        case Direction::Left:  --next.x; break;
        case Direction::Right: ++next.x; break;
        }

        // portals
        {
            const Vec2i head = snake_.head();

            const bool notLocked = (head.x != portalLockCell_.x) || (head.y != portalLockCell_.y);
            if (notLocked) 
            {
                size_t pairIdx = 0; bool isA = false;
                if (isPortalCell(head, &pairIdx, &isA)) 
                {
                    const Vec2i dst = isA ? portals_[pairIdx].b : portals_[pairIdx].a;

                    auto snakeOccupies = [&](int x, int y) 
                        {
                        for (const auto& c : snake_.body()) if (c.x == x && c.y == y) return true;
                        return false;
                        };

                    if (level_.isBlocked(dst.x, dst.y) || snakeOccupies(dst.x, dst.y)) 
                    {
                        die();
                        return;
                    }
                    snake_.teleportHead(dst);
                    portalLockCell_ = dst;
                }
            }
            else 
            {
                const Vec2i h = snake_.head();
                if (h.x != portalLockCell_.x || h.y != portalLockCell_.y)
                    portalLockCell_ = Vec2i(-9999, -9999);
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
                break;
            case AppleKind::Confuse:
                effects_.applyInvert(cfg_.apple.confuseDuration);
                confuseVisT_ = cfg_.apple.confuseDuration;
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
                }
                powerups_.erase(powerups_.begin() + i);
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

// EPH
void PlayState::ephApplyVisible()
{
    for (const auto& p : ephCells_)
        level_.setCell(p.x, p.y, Cell::Solid); 
}

void PlayState::ephApplyHidden()
{
    for (const auto& p : ephCells_)
        level_.setCell(p.x, p.y, Cell::Empty);
}

// h: [0..360), s/v: [0..1]
static sf::Color hsv(float h, float s, float v, sf::Uint8 a = 255) 
{
    h = std::fmodf(h, 360.f); if (h < 0) h += 360.f;
    float c = v * s;
    float x = c * (1.f - std::fabsf(std::fmodf(h / 60.f, 2.f) - 1.f));
    float m = v - c;
    float r = 0, g = 0, b = 0;
    if (h < 60.f) { r = c; g = x; b = 0; }
    else if (h < 120.f) { r = x; g = c; b = 0; }
    else if (h < 180.f) { r = 0; g = c; b = x; }
    else if (h < 240.f) { r = 0; g = x; b = c; }
    else if (h < 300.f) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }
    auto to8 = [](float u) { return (sf::Uint8)std::clamp<int>(int((u) * 255.f + 0.5f), 0, 255); };
    return sf::Color(to8(r + m), to8(g + m), to8(b + m), a);
}

void PlayState::draw(sf::RenderTarget& rt)
{
    const int CELL = cfg_.cellPx;

    // basic cam
    sf::View base = win_.getDefaultView();

    // shake a copy of base cam
    sf::View shaken = base;
    shaken.move(shake_.offsetX(), shake_.offsetY());
    win_.setView(shaken);

    // world render
    sf::RectangleShape rect({ (float)CELL - 1, (float)CELL - 1 });
    for (int y = 0; y < level_.grid().h(); ++y)
    {
        for (int x = 0; x < level_.grid().w(); ++x)
        {
            if (level_.grid().get({ x, y }) == CellType::Wall)
            {
                rect.setPosition((float)(x * CELL), (float)(y * CELL));
                rect.setFillColor(sf::Color(50, 50, 50));
                rt.draw(rect);
            }
        }
    }

    if (gateUnlocked_) 
    {
        // alpha pulse
        gatePulse_ += 0.8f * (1.f / 60.f);
        float a = 180.f + 60.f * std::sin(gatePulse_ * 6.28318f);
        auto col = gateViz_.getFillColor();
        col.a = (sf::Uint8)std::clamp<int>(int(a), 0, 255);
        gateViz_.setFillColor(col);
        rt.draw(gateViz_);
    }

    const float t = portalPulseT_;
    const float freq = 1.5f;
    const float pulseScale = 1.0f + 0.12f * std::sin(2.f * 3.1415926f * freq * t);
    const sf::Uint8 baseA = 150;
    const sf::Uint8 pulseA = (sf::Uint8)(80 * (0.5f + 0.5f * std::sin(2.f * 3.1415926f * freq * t)));

    auto drawCellFilled = [&](const Vec2i& c, sf::Color col) 
        {
        sf::RectangleShape r({ (float)CELL - 2.f, (float)CELL - 2.f });
        r.setPosition((float)c.x * CELL + 1.f, (float)c.y * CELL + 1.f);
        r.setFillColor(col);
        rt.draw(r);
        };

    auto drawPulsingCircle = [&](const Vec2i& c, sf::Color col, bool darker) 
        {
        const float cx = (float)c.x * CELL + CELL * 0.5f;
        const float cy = (float)c.y * CELL + CELL * 0.5f;
        const float radius = (CELL - 6) * 0.5f;

        if (darker) { col.r = (sf::Uint8)(col.r * 0.5f); col.g = (sf::Uint8)(col.g * 0.5f); col.b = (sf::Uint8)(col.b * 0.5f); }
        col.a = (sf::Uint8)std::min<int>(255, baseA + pulseA);

        sf::CircleShape circ(radius);
        circ.setOrigin(radius, radius);
        circ.setPosition(cx, cy);
        circ.setScale(pulseScale, pulseScale);
        circ.setFillColor(col);
        rt.draw(circ);
        };

    for (const auto& p : portals_) 
    {
        // enter brighter
        drawCellFilled(p.a, sf::Color(p.color.r, p.color.g, p.color.b, 180));
        drawPulsingCircle(p.a, p.color, /*darker=*/false);

        // exit darker
        sf::Color outCol(p.color.r / 2, p.color.g / 2, p.color.b / 2);
        drawCellFilled(p.b, sf::Color(outCol.r, outCol.g, outCol.b, 160));
        drawPulsingCircle(p.b, outCol, /*darker=*/true);
    }

    auto prev = rt.getView();
    rt.setView(rt.getDefaultView());
    if (snake_.breakerTimeLeft() > 0.f) 
    {
        rt.draw(brBack_);
        rt.draw(brFill_);
        rt.draw(brText_);
    }
    rt.setView(prev);

    if (apple_) apple_->draw(rt);
    snake_.draw(rt);

    if (confuseVisT_ > 0.f) {
        // color speed
        const float hueBase = std::fmod(confuseHueT_ * 180.f, 360.f);
        int idx = 0;
        for (const auto& cell : snake_.body()) 
        {
            const float h = std::fmod(hueBase + idx * 12.f, 360.f);
            const sf::Color col = hsv(h, 0.85f, 1.0f, 150); // opacity
            sf::RectangleShape r({ (float)CELL - 2.f, (float)CELL - 2.f });
            r.setPosition((float)cell.x * CELL + 1.f, (float)cell.y * CELL + 1.f);
            r.setFillColor(col);
            rt.draw(r);
            ++idx;
        }
    }

    if (apple_) 
    {
        const auto c = apple_->cell();
        const float px = static_cast<float>(c.x * CELL);
        const float py = static_cast<float>(c.y * CELL);

        sf::Color col = sf::Color(200, 200, 200);
        switch (appleKind_) 
        {
        case AppleKind::Bonus:   col = sf::Color(255, 215, 0);   break; // gold
        case AppleKind::Poison:  col = sf::Color(170, 80, 200); break; // purple
        case AppleKind::Confuse: col = sf::Color(80, 200, 200); break; // cyan
        default: break;
        }

        sf::CircleShape appleCircle(static_cast<float>(CELL - 2) * 0.5f);
        appleCircle.setPosition(px + 1.f, py + 1.f);
        appleCircle.setFillColor(col);
        rt.draw(appleCircle);

        if (appleTTL_ > 0.f) 
        {
            float ttlTotal = 0.f;
            switch (appleKind_) 
            {
            case AppleKind::Bonus:   ttlTotal = cfg_.apple.bonusTTL;   break;
            case AppleKind::Poison:  ttlTotal = cfg_.apple.poisonTTL;  break;
            case AppleKind::Confuse: ttlTotal = cfg_.apple.confuseTTL; break;
            default: break;
            }
            if (ttlTotal > 0.f) 
            {
                const float frac = std::clamp(appleTTL_ / ttlTotal, 0.f, 1.f);
                const float w = (CELL - 2) * frac;
                sf::RectangleShape bar({ w, 4.f });
                bar.setPosition(px + 1.f, py - 5.f);
                bar.setFillColor(col);
                rt.draw(bar);
            }
        }
    }

    for (const auto& p : powerups_) 
    {
        const float r = (cfg_.cellPx * 0.7f) * 0.5f;
        sf::CircleShape s(r);
        s.setOrigin(r, r);
        s.setPosition(p.cellX * cfg_.cellPx + cfg_.cellPx * 0.5f,
            p.cellY * cfg_.cellPx + cfg_.cellPx * 0.5f);
        s.setFillColor(sf::Color(255, 215, 0));
        s.setOutlineThickness(2.f);
        s.setOutlineColor(sf::Color::Black);
        rt.draw(s);
    }

    // returns base cam — HUD draws by coords
    win_.setView(base);
    hud_.draw(rt, score_);
    {
        sf::Text info("", res_.font(), 16);
        info.setPosition(12.f, 48.f);
        std::string msg;

        if (effects_.spdRemain() > 0.f) 
        {
            msg += "SPEED x" + std::to_string(effects_.speedMul()) +
                " (" + std::to_string((int)std::ceil(effects_.spdRemain())) + "s)  ";
        }
        if (effects_.invRemain() > 0.f) 
        {
            msg += "CONFUSE (" + std::to_string((int)std::ceil(effects_.invRemain())) + "s)";
        }
        info.setString(msg);
        info.setFillColor(sf::Color(180, 220, 180));
        rt.draw(info);
    }
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
    case AppleKind::Normal:  appleTTL_ = 0.f;                         break;
    case AppleKind::Bonus:   appleTTL_ = cfg_.apple.bonusTTL;         break;
    case AppleKind::Poison:  appleTTL_ = cfg_.apple.poisonTTL;        break;
    case AppleKind::Confuse: appleTTL_ = cfg_.apple.confuseTTL;       break;
    }
}

void PlayState::die()
{
    sfxDeath_.play();
    sm_.push(std::make_unique<GameOverState>(sm_, win_, cfg_, res_, score_.value()));
    res_.switchToGameOver();
}

void PlayState::flashSnake() 
{}