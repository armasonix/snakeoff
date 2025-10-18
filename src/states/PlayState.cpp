#include "states/PlayState.h"
#include "systems/Spawner.h"
#include "systems/Collision.h"
#include "states/PauseState.h"
#include "states/GameOverState.h"
#include "world/ProcGen.h"
#include <SFML/Graphics.hpp>
#include <functional>

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
        if (e.key.code == sf::Keyboard::W) snake_.setDirection(Direction::Up);
        if (e.key.code == sf::Keyboard::S) snake_.setDirection(Direction::Down);
        if (e.key.code == sf::Keyboard::A) snake_.setDirection(Direction::Left);
        if (e.key.code == sf::Keyboard::D) snake_.setDirection(Direction::Right);
        if (e.key.code == sf::Keyboard::P) 
        {
            sm_.push(std::make_unique<class PauseState>(sm_, win_, cfg_, res_));
        }
    }
}

void PlayState::update(float dt) 
{
    shake_.update(dt);
    snake_.update(dt);

    if (startDelay_ > 0.f) { startDelay_ -= dt; return; }

    timeAcc_ += dt;
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
            spawnApple();
            // slight shake
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

    // returns base cam — HUD draws by coords
    win_.setView(base);
    hud_.draw(rt, score_);
}

void PlayState::spawnApple() 
{
    apple_ = Spawner::spawnNormal(level_.grid(), snake_);
}

void PlayState::die() 
{
    sfxDeath_.play();
    // transfer to death screen
    sm_.push(std::make_unique<class GameOverState>(sm_, win_, cfg_, res_, score_.value()));
}

void PlayState::flashSnake() 
{}