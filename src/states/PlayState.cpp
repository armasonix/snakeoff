#include "states/PlayState.h"
#include "systems/Spawner.h"
#include "systems/Collision.h"
#include "states/PauseState.h"
#include "states/GameOverState.h"
#include "world/ProcGen.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <cmath>

PlayState::PlayState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
    : sm_(sm), win_(win), cfg_(cfg), res_(res),
    level_(Level::loadFromFile("data/levels/level1.txt", cfg.gridWidth, cfg.gridHeight)),
    snake_({ cfg.gridWidth / 2, cfg.gridHeight / 2 }),
    hud_(res.font())
{
    sfxEat_.setBuffer(res_.sfxEat());
    sfxDeath_.setBuffer(res_.sfxDeath());
}

void PlayState::onEnter() 
{
    win_.setView(win_.getDefaultView());
    effects_.reset(); // clear effects
    appleKind_ = AppleKind::Normal;
    appleTTL_ = 0.f;
    level_.clear();
    level_.buildBorders();

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

    spawnApple();
}

void PlayState::handleEvent(const sf::Event& e) 
{
    if (e.type == sf::Event::KeyPressed)
    {
        auto key = e.key.code;
        if (effects_.inverted())
        {
            // invert W<->S, A<->D
            if (key == sf::Keyboard::W) key = sf::Keyboard::S;
            else if (key == sf::Keyboard::S) key = sf::Keyboard::W;
            else if (key == sf::Keyboard::A) key = sf::Keyboard::D;
            else if (key == sf::Keyboard::D) key = sf::Keyboard::A;
        }

        if (key == sf::Keyboard::W) snake_.setDirection(Direction::Up);
        if (key == sf::Keyboard::S) snake_.setDirection(Direction::Down);
        if (key == sf::Keyboard::A) snake_.setDirection(Direction::Left);
        if (key == sf::Keyboard::D) snake_.setDirection(Direction::Right);

        if (key == sf::Keyboard::P)
        {
            sm_.push(std::make_unique<class PauseState>(sm_, win_, cfg_, res_));
        }
    }
}

void PlayState::update(float dt) 
{
    effects_.update(dt);
    shake_.update(dt);
    snake_.update(dt);

    if (startDelay_ > 0.f) { startDelay_ -= dt; return; }

    if (appleKind_ == AppleKind::Bonus && appleTTL_ > 0.f)
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

    const float step = cfg_.paramsFor(cfg_.difficulty).stepSec;
    while (timeAcc_ >= step) 
    {
        timeAcc_ -= step;
        snake_.step();

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
                break;
            default:
                break;
            }
            spawnApple();
            shake_.start(0.12f, 2.0f);
        }
    }
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

    if (apple_) apple_->draw(rt);
    snake_.draw(rt);

    if (apple_) 
    {
        const int CELL = cfg_.cellPx;

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

        if (appleKind_ == AppleKind::Bonus && appleTTL_ > 0.f) 
        {
            const float ttl = std::max(0.001f, cfg_.apple.bonusTTL);
            const float frac = std::clamp(appleTTL_ / ttl, 0.f, 1.f);
            const float w = (CELL - 2) * frac;

            sf::RectangleShape bar({ w, 4.f });
            bar.setPosition(px + 1.f, py - 5.f);
            bar.setFillColor(col);
            rt.draw(bar);
        }
    }

    // returns base cam — HUD draws by coords
    win_.setView(base);
    hud_.draw(rt, score_);
    {
        sf::Text info("", res_.font(), 16);
        info.setPosition(12.f, 48.f);
        std::string s;

        if (effects_.spdRemain() > 0.f) 
        {
            s += "SPEED x" + std::to_string(effects_.speedMul()) +
                " (" + std::to_string((int)std::ceil(effects_.spdRemain())) + "s)  ";
        }
        if (effects_.invRemain() > 0.f) 
        {
            s += "CONFUSE (" + std::to_string((int)std::ceil(effects_.invRemain())) + "s)";
        }
        info.setString(s);
        info.setFillColor(sf::Color(180, 220, 180));
        rt.draw(info);
    }
}

void PlayState::spawnApple()
{
    // basic spawn
    apple_ = Spawner::spawnNormal(level_.grid(), snake_);

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
    appleTTL_ = (appleKind_ == AppleKind::Bonus) ? cfg_.apple.bonusTTL : 0.f;
}

void PlayState::die() 
{
    sfxDeath_.play();
    // transfer to death screen
    sm_.push(std::make_unique<class GameOverState>(sm_, win_, cfg_, res_, score_.value()));
}

void PlayState::flashSnake() 
{}