#include "states/PlayState.h"
#include "systems/Spawner.h"
#include "systems/Collision.h"
#include "states/PauseState.h"
#include "states/GameOverState.h"
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
    score_.reset();
    spawnApple();
    startDelay_ = cfg_.startDelaySec; // start delay T
    timeAcc_ = 0.f;
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
    // camera shake
    sf::View view = win_.getView();
    view.move(shake_.offsetX(), shake_.offsetY());
    win_.setView(view);

    // draw walls
    sf::RectangleShape rect({ (float)CELL - 1,(float)CELL - 1 });
    for (int y = 0;y < level_.grid().h();++y)
    {
        for (int x = 0;x < level_.grid().w();++x) 
        {
            if (level_.grid().get({ x,y }) == CellType::Wall) 
            {
                rect.setPosition((float)x * CELL, (float)y * CELL);
                rect.setFillColor(sf::Color(50, 50, 50));
                rt.draw(rect);
            }
        }
    }

    // draws
    if (apple_) apple_->draw(rt);
    snake_.draw(rt);
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