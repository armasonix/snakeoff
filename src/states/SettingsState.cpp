#include "states/SettingsState.h"
#include "core/StateMachine.h"
#include <algorithm>

SettingsState::SettingsState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
    : sm_(sm), win_(win), cfg_(cfg), res_(res) {}

void SettingsState::onEnter() 
{
    res_.ensureMenuLoop();
}

void SettingsState::toggleCurrent() 
{
    if (selected_ == Sound) 
    {
        cfg_.soundOn = !cfg_.soundOn;
        res_.setSoundEnabled(cfg_.soundOn);
    }
    else if (selected_ == Music)
    {
        cfg_.musicOn = !cfg_.musicOn;
        res_.setMusicEnabled(cfg_.musicOn);
    }
    cfg_.saveUserSettings("data/settings.cfg");
}

void SettingsState::handleEvent(const sf::Event& e) 
{
    if (e.type != sf::Event::KeyPressed) return;
    switch (e.key.code) 
    {
    case sf::Keyboard::W:
    case sf::Keyboard::Up:
        selected_ = (selected_ - 1 + Count) % Count;
        break;
    case sf::Keyboard::S:
    case sf::Keyboard::Down:
        selected_ = (selected_ + 1) % Count;
        break;
    case sf::Keyboard::Enter:
        toggleCurrent();
        break;
    case sf::Keyboard::B:
    case sf::Keyboard::Escape:
        sm_.pop();
        break;
    default: break;
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
}