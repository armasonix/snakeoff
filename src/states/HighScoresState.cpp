#include "states/HighScoresState.h"
#include "core/StateMachine.h"

HighScoresState::HighScoresState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
    : sm_(sm), win_(win), cfg_(cfg), res_(res)
{
    sfxHit_.setBuffer(res_.sfxUiHit());
}

void HighScoresState::onEnter() 
{
    hs_.load(path_);
    res_.ensureMenuLoop();
}

void HighScoresState::handleEvent(const sf::Event& e) 
{
    if (e.type == sf::Event::KeyPressed) 
    {
        if (e.key.code == sf::Keyboard::Enter || e.key.code == sf::Keyboard::B) 
        {
            sm_.pop(); // back to menu
            sfxHit_.play();
        }
        if (e.key.code == sf::Keyboard::Escape)
        {
            sfxHit_.play();
            sm_.pop();
        }
    }
}

void HighScoresState::draw(sf::RenderTarget& rt) 
{
    sf::Text title("High Scores", res_.font(), 28);
    title.setPosition(60, 40);
    title.setFillColor(sf::Color::White);
    rt.draw(title);

    auto list = hs_.topN((size_t)cfg_.tableRowsY);
    sf::Text row("", res_.font(), 22);
    float y = 90.f;
    for (size_t i = 0; i < list.size(); ++i) 
    {
        row.setPosition(60, y);
        row.setString(std::to_string((int)i + 1) + ". " + list[i].name + " — " + std::to_string(list[i].score));
        row.setFillColor(sf::Color::Green);
        rt.draw(row);
        y += 28.f;
    }

    sf::Text hint("Enter/B or Esc - back", res_.font(), 18);
    hint.setPosition(60, y + 20.f);
    hint.setFillColor(sf::Color(180, 180, 180));
    rt.draw(hint);
}