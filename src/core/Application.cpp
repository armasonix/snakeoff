#include "core/Application.h"
#include "states/IGameState.h"
#include "states/MenuState.h"

Application::Application(int wPx, int hPx)
    : win_(sf::VideoMode(wPx, hPx), "SnakeOff", sf::Style::Close) 
{
    win_.setVerticalSyncEnabled(true);
}

bool Application::init() 
{
    const int W = cfg_.gridWidth * cfg_.cellPx;
    const int H = cfg_.gridHeight * cfg_.cellPx;
    const float s = cfg_.windowScale; // default scale
    win_.setSize({ (unsigned)(W * s), (unsigned)(H * s) });
    sf::View worldView(sf::FloatRect(0, 0, (float)W, (float)H));
    win_.setView(worldView);

    cfg_.loadUserSettings("data/settings.cfg");
    res_.setSoundEnabled(cfg_.soundOn);
    res_.setMusicEnabled(cfg_.musicOn);

    return res_.load("assets");
}

void Application::run() 
{
    sm_.push(std::make_unique<MenuState>(sm_, win_, cfg_, res_));

    sf::Clock clock;
    while (win_.isOpen()) 
    {
        sf::Event e;
        while (win_.pollEvent(e)) 
        {
            if (e.type == sf::Event::Closed) win_.close();
            if (auto* gs = dynamic_cast<IGameState*>(sm_.top()))
                gs->handleEvent(e);
        }
        float dt = clock.restart().asSeconds();
        if (auto* gs = dynamic_cast<IGameState*>(sm_.top())) gs->update(dt);

        win_.clear(sf::Color(15, 15, 18));
        if (auto* gs = dynamic_cast<IGameState*>(sm_.top())) gs->draw(win_);
        win_.display();
    }
}