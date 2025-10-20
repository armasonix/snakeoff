#include "states/MenuState.h"
#include "states/PlayState.h"
#include "core/StateMachine.h"
#include "core/Resources.h" 
#include <SFML/Graphics.hpp>
#include "states/SettingsState.h"
#include "states/DifficultyState.h"
#include "states/HighScoresState.h"

MenuState::MenuState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
    : sm_(sm), win_(win), cfg_(cfg), res_(res) 
{
    sfxMove_.setBuffer(res_.sfxUiMove());
    sfxHit_.setBuffer(res_.sfxUiHit());
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
        else if (selected_ == 2) sm_.push(std::make_unique<SettingsState>(sm_, win_, cfg_, res_));
        else if (selected_ == 3) sm_.push(std::make_unique<HighScoresState>(sm_, win_, cfg_, res_));
        else if (selected_ == 4) win_.close();
        sfxHit_.play();
        break;

    default: break;
    }
}
void MenuState::update(float) {}

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
    sf::Text t; t.setFont(res_.font()); t.setCharacterSize(28);
    for (int i = 0;i < (int)items_.size();++i) 
    {
        t.setString(items_[i]);
        t.setFillColor(i == selected_ ? sf::Color::Green : sf::Color::White);
        t.setPosition(60.f, 80.f + i * 40.f);
        rt.draw(t);
    }
    sf::Text d; d.setFont(res_.font()); d.setCharacterSize(18);
    d.setPosition(60.f, 40.f);
    d.setString("Difficulty: " + std::to_string((int)cfg_.difficulty));
    rt.draw(d);
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