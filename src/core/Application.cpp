#include "Application.h"
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
    win_.setSize({ (unsigned)W,(unsigned)H });
    win_.setView(sf::View(sf::FloatRect(0, 0, (float)W, (float)H)));
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
            if (auto* gs = dynamic_cast<class IGameState*>(sm_.top()))
                gs->handleEvent(e);
        }
        float dt = clock.restart().asSeconds();
        if (auto* gs = dynamic_cast<class IGameState*>(sm_.top())) gs->update(dt);

        win_.clear(sf::Color(15, 15, 18));
        if (auto* gs = dynamic_cast<class IGameState*>(sm_.top())) gs->draw(win_);
        win_.display();
    }
}