#include "states/GameOverState.h"
#include "states/PlayState.h"
#include "states/MenuState.h"
#include "core/StateMachine.h"
#include <SFML/Graphics.hpp>

void GameOverState::onEnter() 
{
    hs_.load(path_);
    askSel_ = 0;
    optSel_ = 0;
    phase_ = Phase::AskSave;
    sfxMove_.setBuffer(res_.sfxUiMove());
    sfxHit_.setBuffer(res_.sfxUiHit());
}

void GameOverState::handleEvent(const sf::Event& e) 
{
    if (phase_ == Phase::AskSave) 
    {
        if (e.type == sf::Event::KeyPressed) 
        {
            if (e.key.code == sf::Keyboard::W || e.key.code == sf::Keyboard::S
            || e.key.code == sf::Keyboard::Up || e.key.code == sf::Keyboard::Down)
                askSel_ = 1 - askSel_;
                sfxMove_.play();
            if (e.key.code == sf::Keyboard::Enter) 
            {
                if (askSel_ == 1) phase_ = Phase::EnterName; // yes - enter name
                else             phase_ = Phase::Options;   // no - to options
                sfxHit_.play();
            }
        }
        return;
    }

    if (phase_ == Phase::EnterName) 
    {
        if (e.type == sf::Event::TextEntered) 
        {
            auto ch = static_cast<uint32_t>(e.text.unicode);
            if (ch >= 32 && ch <= 126 && name_.size() < 16) 
            {
                name_.push_back(static_cast<char>(ch));
            }
        }
        else if (e.type == sf::Event::KeyPressed) 
        {
            if (e.key.code == sf::Keyboard::BackSpace && !name_.empty()) 
            {
                name_.pop_back();
                sfxHit_.play();
            }
            if (e.key.code == sf::Keyboard::Enter) 
            {
                std::string nm = name_.empty() ? std::string("ABC") : name_;
                hs_.submit(nm, score_);
                hs_.save(path_);
                phase_ = Phase::Options;
                sfxHit_.play();
            }
            if (e.key.code == sf::Keyboard::B || e.key.code == sf::Keyboard::Escape)
            {
                phase_ = Phase::AskSave; // back to selection yes/no
                sfxHit_.play();
            }
        }
        return;
    }

    // Phase::Options - select start game / main menu
    if (e.type == sf::Event::KeyPressed) 
    {
        if (e.key.code == sf::Keyboard::W || e.key.code == sf::Keyboard::S
        || e.key.code == sf::Keyboard::Up || e.key.code == sf::Keyboard::Down)
            optSel_ = 1 - optSel_;
            sfxMove_.play();
        if (e.key.code == sf::Keyboard::Enter) 
        {
            if (optSel_ == 0) sm_.push(std::make_unique<PlayState>(sm_, win_, cfg_, res_));
            else             sm_.push(std::make_unique<MenuState>(sm_, win_, cfg_, res_));
            sfxHit_.play();
        }
    }
}

void GameOverState::draw(sf::RenderTarget& rt) 
{
    sf::Text t; t.setFont(res_.font()); t.setCharacterSize(28);
    t.setFillColor(sf::Color::White);
    t.setPosition(60, 40);
    t.setString("Game Over!\nScore: " + std::to_string(score_));
    rt.draw(t);

    // Top X right side
    auto list = hs_.topN((size_t)cfg_.popupRowsX);
    sf::Text hdr("Highlights (top " + std::to_string(cfg_.popupRowsX) + ")", res_.font(), 22);
    hdr.setFillColor(sf::Color(200, 200, 255));
    hdr.setPosition(420, 40);
    rt.draw(hdr);

    sf::Text row("", res_.font(), 20);
    float y = 80.f;
    for (size_t i = 0; i < list.size(); ++i) 
    {
        row.setPosition(420, y);
        row.setString(std::to_string((int)i + 1) + ". " + list[i].name + " — " + std::to_string(list[i].score));
        row.setFillColor(sf::Color(120, 220, 120));
        rt.draw(row);
        y += 24.f;
    }

    if (phase_ == Phase::AskSave) 
    {
        sf::Text q("Save result?", res_.font(), 24);
        q.setPosition(60, 140); q.setFillColor(sf::Color::White); rt.draw(q);

        sf::Text a("No", res_.font(), 22);
        sf::Text b("Yes", res_.font(), 22);
        a.setPosition(60, 180); b.setPosition(140, 180);
        a.setFillColor(askSel_ == 0 ? sf::Color::Green : sf::Color::White);
        b.setFillColor(askSel_ == 1 ? sf::Color::Green : sf::Color::White);
        rt.draw(a); rt.draw(b);

        sf::Text hint("W/S or Up/Down - move, Enter - select", res_.font(), 16);
        hint.setPosition(60, 220); hint.setFillColor(sf::Color(180, 180, 180)); rt.draw(hint);
        return;
    }

    if (phase_ == Phase::EnterName) 
    {
        sf::Text q("Type name (Enter - save, B/Esc - back):", res_.font(), 22);
        q.setPosition(60, 140); q.setFillColor(sf::Color::White); rt.draw(q);

        sf::Text nameText(name_.empty() ? "ABC" : name_, res_.font(), 26);
        nameText.setPosition(60, 180);
        nameText.setFillColor(sf::Color::Yellow);
        rt.draw(nameText);
        return;
    }

    // Phase::Options
    sf::Text o1("Start Game", res_.font(), 24);
    sf::Text o2("Main Menu", res_.font(), 24);
    o1.setPosition(60, 180); o2.setPosition(60, 220);
    o1.setFillColor(optSel_ == 0 ? sf::Color::Green : sf::Color::White);
    o2.setFillColor(optSel_ == 1 ? sf::Color::Green : sf::Color::White);
    rt.draw(o1); rt.draw(o2);

    sf::Text hint("W/S or Up/Down - move, Enter - select", res_.font(), 16);
    hint.setPosition(60, 260); hint.setFillColor(sf::Color(180, 180, 180)); rt.draw(hint);
}