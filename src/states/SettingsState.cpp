#include "states/SettingsState.h"
#include "core/StateMachine.h"
#include <algorithm>

SettingsState::SettingsState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
    : sm_(sm), win_(win), cfg_(cfg), res_(res) 
{
    sfxMove_.setBuffer(res_.sfxUiMove());
    sfxHit_.setBuffer(res_.sfxUiHit());

    items_ = 
    {
        {"Sound",& cfg_.soundOn},
        {"Music",& cfg_.musicOn},
        {"Obstacles",& cfg_.ephemeralObstacles}
    };
}

void SettingsState::onEnter() 
{
    res_.ensureMenuLoop();
}

void SettingsState::toggleCurrent() 
{
    switch (selected_)
    {
    case Sound:
        cfg_.soundOn = !cfg_.soundOn;
        res_.setSoundEnabled(cfg_.soundOn);
        break;
    case Music:
        cfg_.musicOn = !cfg_.musicOn;
        res_.setMusicEnabled(cfg_.musicOn);
        break;
    case Eph:
    default:
        cfg_.ephemeralObstacles = !cfg_.ephemeralObstacles;
        break;
    }
    cfg_.saveUserSettings("data/settings.cfg");
}

void SettingsState::handleEvent(const sf::Event& e) 
{
    if (e.type == sf::Event::KeyPressed) 
    {
        const auto code = e.key.code;

        if (code == sf::Keyboard::Up || code == sf::Keyboard::W) 
        {
            selected_ = (selected_ + (int)items_.size() - 1) % (int)items_.size();
            sfxMove_.play();
        }
        if (code == sf::Keyboard::Down || code == sf::Keyboard::S) 
        {
            selected_ = (selected_ + 1) % (int)items_.size();
            sfxMove_.play();
        }
        if (code == sf::Keyboard::Enter) 
        {
            toggleCurrent();
            sfxHit_.play();
        }
        if (e.key.code == sf::Keyboard::B || code == sf::Keyboard::Escape)
        {
            sfxHit_.play();
            sm_.pop(); // back to menu
        }
    }
}

void SettingsState::update(float) {}

static sf::Text T(const sf::Font& f, unsigned size, sf::Color c, const std::string& s, float x, float y) 
{
    sf::Text t;
    t.setFont(f); t.setCharacterSize(size); t.setFillColor(c); t.setString(s); t.setPosition(x, y);
    return t;
}

void SettingsState::draw(sf::RenderTarget& rt) 
{
    const auto& font = res_.font();
    const auto view = rt.getView();
    const auto pos = view.getCenter() - 0.5f * view.getSize();

    rt.draw(T(font, 36, sf::Color::White, "Settings", pos.x + 40.f, pos.y + 40.f));
    rt.draw(T(font, 18, sf::Color(200, 200, 200), "W/S or Up/Down - move, Enter select, B/Esc - back", pos.x + 40.f, pos.y + 90.f));

    float y = pos.y + 150.f;
    auto row = [&](const char* caption, bool on, bool sel) 
        {
        const auto color = sel ? sf::Color(80, 255, 120) : sf::Color(230, 230, 230);
        rt.draw(T(font, 26, color, caption, pos.x + 60.f, y));
        rt.draw(T(font, 26, on ? sf::Color(120, 220, 120) : sf::Color(220, 120, 120),
            on ? "ON" : "OFF", pos.x + 420.f, y));
        y += 48.f;
        };

    row("Sound", cfg_.soundOn, selected_ == Sound);
    row("Music", cfg_.musicOn, selected_ == Music);
    row("Obstacles", cfg_.ephemeralObstacles, selected_ == Eph);
}