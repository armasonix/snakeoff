#include "states/HighScoresState.h"
#include "core/StateMachine.h"

HighScoresState::HighScoresState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
    : sm_(sm), win_(win), cfg_(cfg), res_(res) {}

void HighScoresState::onEnter() 
{
    hs_.load(path_);
    res_.ensureMenuLoop();
    sfxHit_.setBuffer(res_.sfxUiHit());
}

void HighScoresState::handleEvent(const sf::Event& e) 
{
    if (e.type == sf::Event::KeyPressed) 
    {
        if (e.key.code == sf::Keyboard::Enter || e.key.code == sf::Keyboard::B) 
        {
            sfxHit_.play();
            sm_.pop(); // back to menu
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
    auto prev = rt.getView();
    rt.setView(rt.getDefaultView());

    const sf::Vector2f center = win_.getView().getCenter();
    const float cx = std::floor(center.x);
    
    sf::Text title("High Scores", res_.font(), 40);
    title.setFillColor(sf::Color::White);
    {
        const auto b = title.getLocalBounds();
        title.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
        title.setPosition(cx, std::floor(center.y - 160.f));
        rt.draw(title);
    }

    auto list = hs_.topN((size_t)cfg_.tableRowsY);
    sf::Text row("", res_.font(), 24);
    const float startY = std::floor(center.y - 80.f);
    const float step = 32.f;
    for (size_t i = 0; i < list.size(); ++i) 
    {
        row.setString(std::to_string((int)i + 1) + ". " + list[i].name + "  >  " + std::to_string(list[i].score));
        row.setFillColor(sf::Color::Green);
        const auto rb = row.getLocalBounds();
        row.setOrigin(rb.left + rb.width * 0.5f, rb.top + rb.height * 0.5f);
        row.setPosition(cx, std::floor(startY + (float)i * step));
        rt.draw(row);
    }

    sf::Text hint("Enter/B or Esc - back", res_.font(), 18);
    hint.setFillColor(sf::Color(180, 180, 180));
    {
        const auto hb = hint.getLocalBounds();
        hint.setOrigin(hb.left + hb.width * 0.5f, hb.top + hb.height * 0.5f);
        hint.setPosition(cx, std::floor(startY + (float)list.size() * step + 40.f));
        rt.draw(hint);
    }
    
    rt.setView(prev);
}