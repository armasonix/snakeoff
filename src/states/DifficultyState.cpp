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
    if (e.type != sf::Event::KeyPressed) return;

    const auto key = e.key.code;
    if (key == sf::Keyboard::W) { selected_ = (selected_ - 1 + (int)items_.size()) % (int)items_.size(); }
    if (key == sf::Keyboard::S) { selected_ = (selected_ + 1) % (int)items_.size(); }

    if (key == sf::Keyboard::Enter)
    {
        cfg_.difficulty = items_[selected_].d;
        sm_.pop();
    }
    if (key == sf::Keyboard::B)
    {
        sm_.pop();
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
    title.setString("Difficulty select");
    title.setPosition(40.f, 40.f);
    rt.draw(title);

    // help
    sf::Text hint;
    hint.setFont(font);
    hint.setCharacterSize(18);
    hint.setFillColor(sf::Color(200, 200, 200));
    hint.setString("W/S - move    Enter - select    B - back");
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