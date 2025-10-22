#include "states/PlayState.h"
#include "systems/Spawner.h"
#include "systems/Collision.h"
#include "states/PauseState.h"
#include "states/GameOverState.h"
#include "core/StateMachine.h"
#include "world/ProcGen.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <algorithm>
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

    // EPH
    ephObstacles_ = obstacles;
    ephVisible_ = true;
    ephTimer_ = 0.f;
    level_.applyObstacles(ephObstacles_);

    spawnPortals(1);
    spawnApple();
    initialized_ = true;
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
    if (e.type == sf::Event::KeyPressed)
    {
        auto key = e.key.code;
        if (effects_.inverted())
        {
            // invert W<->S, A<->D
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

    const float step = cfg_.paramsFor(cfg_.difficulty).stepSec;
    while (timeAcc_ >= step) 
    {
        timeAcc_ -= step;
        snake_.step();

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