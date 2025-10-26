#include "states/MenuState.h"
#include "states/PlayState.h"
#include "core/StateMachine.h"
#include "core/Resources.h" 
#include <SFML/Graphics.hpp>
#include "states/SettingsState.h"
#include "states/DifficultyState.h"
#include "states/HighScoresState.h"
#include <cmath>

// HSV to RGB helper
static sf::Color hsv(float h, float s, float v)
{
    h = std::fmod(h, 360.f); if (h < 0) h += 360.f;
    float c = v * s;
    float x = c * (1 - std::fabs(std::fmod(h / 60.f, 2.f) - 1));
    float m = v - c;
    float r = 0, g = 0, b = 0;
    if (h < 60) { r = c; g = x; b = 0; }
        else if (h < 120) { r = x; g = c; b = 0; }
        else if (h < 180) { r = 0; g = c; b = x; }
        else if (h < 240) { r = 0; g = x; b = c; }
        else if (h < 300) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }
    return sf::Color(
    (sf::Uint8)std::round((r + m) * 255.f),
    (sf::Uint8)std::round((g + m) * 255.f),
    (sf::Uint8)std::round((b + m) * 255.f));
}

MenuState::MenuState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
    : sm_(sm), win_(win), cfg_(cfg), res_(res) 
{
    sfxMove_.setBuffer(res_.sfxUiMove());
    sfxHit_.setBuffer(res_.sfxUiHit());

    titleText_.setFont(res_.font());
    titleText_.setString(L"SNAKEOFF");
    titleText_.setCharacterSize(72);
    titleText_.setOutlineThickness(2.f);
    titleText_.setOutlineColor(sf::Color::White);
    titleText_.setFillColor(sf::Color::White);
}

void MenuState::onEnter()
{
    res_.ensureMenuLoop();
}

void MenuState::handleEvent(const sf::Event& e)
{
    if (e.type != sf::Event::KeyPressed) return;

    switch (e.key.code)
    {
    case sf::Keyboard::W:
    case sf::Keyboard::Up:
        selected_ = (selected_ + (int)items_.size() - 1) % (int)items_.size();
        sfxMove_.play();
        break;

    case sf::Keyboard::S:
    case sf::Keyboard::Down:
        selected_ = (selected_ + 1) % (int)items_.size();
        sfxMove_.play();
        break;

    case sf::Keyboard::Enter:
        if (selected_ == 0) sm_.push(std::make_unique<PlayState>(sm_, win_, cfg_, res_));
        else if (selected_ == 1) sm_.push(std::make_unique<DifficultyState>(sm_, win_, cfg_, res_));
        else if (selected_ == 3) sm_.push(std::make_unique<SettingsState>(sm_, win_, cfg_, res_));
        else if (selected_ == 2) sm_.push(std::make_unique<HighScoresState>(sm_, win_, cfg_, res_));
        else if (selected_ == 4) win_.close();
        sfxHit_.play();
        break;

    default: break;
    }
}
void MenuState::update(float dt)
{
    titleHue_ += titleHueSpeed_ * dt;
    if (titleHue_ >= 360.f) titleHue_ -= 360.f;
    titleText_.setFillColor(hsv(titleHue_, 1.f, 1.f));
}

static const char* difficultyName(Difficulty d)
{
    switch (d)
    {
    case Difficulty::D1: return "easy peasy";
    case Difficulty::D2: return "i am just walkaround";
    case Difficulty::D3: return "hold on my cup boy";
    case Difficulty::D4: return "lets rock";
    case Difficulty::D5: return "we are already dead";
    }
    return "unknown";
}

void MenuState::draw(sf::RenderTarget& rt) 
{
    auto prev = rt.getView();
    rt.setView(rt.getDefaultView());

    const sf::Vector2f center = win_.getView().getCenter();
    const float cx = std::floor(center.x);
        {
        const auto b = titleText_.getLocalBounds();
        titleText_.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
        titleText_.setPosition(cx, std::floor(center.y - 160.f));
        rt.draw(titleText_);
        }
        sf::Text t; t.setFont(res_.font()); t.setCharacterSize(28);
    const sf::Color kMenuGreen(120, 220, 120);
    const float startY = std::floor(center.y - 20.f);
    const float step = 48.f;
    for (int i = 0; i < (int)items_.size(); ++i)
        {
        t.setString(items_[i]);
        const auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
        t.setPosition(cx, std::floor(startY + i * step));
        t.setStyle(i == selected_ ? sf::Text::Bold : sf::Text::Regular);
        t.setFillColor(i == selected_ ? kMenuGreen : sf::Color::White);
        rt.draw(t);
        }

    sf::Text d; d.setFont(res_.font()); d.setCharacterSize(18);
    d.setPosition(60.f, 40.f);
    d.setString("Your Difficulty: " + std::to_string((int)cfg_.difficulty));
    rt.draw(d);

    rt.setView(prev);
}

void MenuState::startGame() 
{
    // change current state for game
    sm_.push(std::make_unique<class PlayState>(sm_, win_, cfg_, res_));
}

void MenuState::changeDifficulty() 
{
    int cur = (int)cfg_.difficulty;
    cur = (cur % 5) + 1;
    cfg_.difficulty = (Difficulty)cur;
}