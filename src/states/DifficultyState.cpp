#include "states/DifficultyState.h"
#include "core/StateMachine.h"
#include "states/PlayState.h"
#include <algorithm>
#include <cstdio>

DifficultyState::DifficultyState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res)
    : sm_(sm), win_(win), cfg_(cfg), res_(res)
{
    items_ = 
    {
        { Difficulty::D1, "1 - easy peasy",          "slow, +2 scores for apple"        },
        { Difficulty::D2, "2 - i am just walkaround","faster, +4 scores for apple"      },
        { Difficulty::D3, "3 - hold on my cup boy",  "medium speed, +6 scores for apple"},
        { Difficulty::D4, "4 - lets rock",           "fast, +8 scores for apple"        },
        { Difficulty::D5, "5 - we are already dead", "very fast, +10 scores for apple"  },
    };
    sfxMove_.setBuffer(res_.sfxUiMove());
    sfxHit_.setBuffer(res_.sfxUiHit());
}

int DifficultyState::indexFromDifficulty(Difficulty d) const
{
    const int idx = std::clamp(int(d) - 1, 0, int(items_.size()) - 1);
    return idx;
}

void DifficultyState::onEnter()
{
    selected_ = indexFromDifficulty(cfg_.difficulty);
    res_.ensureMenuLoop();
}

void DifficultyState::handleEvent(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        if (e.key.code == sf::Keyboard::W || e.key.code == sf::Keyboard::Up)
        {
            selected_ = (selected_ + (int)items_.size() - 1) % (int)items_.size();
            sfxMove_.play();
        }
        else if (e.key.code == sf::Keyboard::S || e.key.code == sf::Keyboard::Down)
        {
            selected_ = (selected_ + 1) % (int)items_.size();
            sfxMove_.play();
        }
        else if (e.key.code == sf::Keyboard::Enter)
        {
            cfg_.difficulty = (Difficulty)selected_;
            sm_.push(std::make_unique<PlayState>(sm_, win_, cfg_, res_));
            sfxHit_.play();
        }
        else if (e.key.code == sf::Keyboard::B || e.key.code == sf::Keyboard::Escape)
        {
            sfxHit_.play();
            sm_.pop();
        }
    }
}

void DifficultyState::update(float) {}

void DifficultyState::draw(sf::RenderTarget& rt)
{
    const sf::Font& font = res_.font();

    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(36);
    title.setFillColor(sf::Color::White);
    title.setString("Select Your Difficulty");
    title.setPosition(40.f, 40.f);
    rt.draw(title);

    // help
    sf::Text hint;
    hint.setFont(font);
    hint.setCharacterSize(18);
    hint.setFillColor(sf::Color(200, 200, 200));
    hint.setString("W/S Up/Down - move    Enter - select    B/Esc - back");
    hint.setPosition(40.f, 88.f);
    rt.draw(hint);

    // Difficulty list
    float y = 140.f;
    for (int i = 0; i < (int)items_.size(); ++i)
    {
        const bool sel = (i == selected_);

        sf::Text line;
        line.setFont(font);
        line.setCharacterSize(28);
        line.setFillColor(sel ? sf::Color(80, 255, 120) : sf::Color(230, 230, 230));
        line.setString(items_[i].name);
        line.setPosition(60.f, y);
        rt.draw(line);

        const auto p = cfg_.paramsFor(items_[i].d);
        char buf[128];
        std::snprintf(buf, sizeof(buf), "%s  |  step: %.0f ms, scores per apple: %d",
            items_[i].tagline, p.stepSec * 1000.f, p.pointsPerApple);

        sf::Text sub;
        sub.setFont(font);
        sub.setCharacterSize(18);
        sub.setFillColor(sf::Color(150, 150, 150));
        sub.setString(buf);
        sub.setPosition(80.f, y + 30.f);
        rt.draw(sub);

        y += 64.f;
    }
}