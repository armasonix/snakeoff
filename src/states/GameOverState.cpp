#include "states/GameOverState.h"
#include "states/PlayState.h"
#include "states/MenuState.h"
#include "core/StateMachine.h"
#include <SFML/Graphics.hpp>
#include <cmath>

//HSV to RGB helper
static sf::Color hsv(float h, float s, float v)
{
    h = std::fmod(h, 360.f); if (h < 0) h += 360.f;
    const float c = v * s;
    const float x = c * (1.f - std::fabs(std::fmod(h / 60.f, 2.f) - 1.f));
    const float m = v - c;
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

void GameOverState::update(float dt) 
{
    titleHue_ += titleHueSpeed_ * dt;
    if (titleHue_ >= 120.f) titleHue_ -= 120.f;
}

void GameOverState::draw(sf::RenderTarget& rt)
{
    auto prev = rt.getView();
    rt.setView(rt.getDefaultView());

    const sf::Vector2f center = win_.getView().getCenter();
    const float cx = std::floor(center.x);

    sf::Text title; title.setFont(res_.font());
    title.setCharacterSize(48);
    title.setString("Game Over!");
    
    float h = std::fmod(titleHue_, 120.f);
    if (h > 60.f) h = 120.f - h;
    title.setFillColor(hsv(h, 1.f, 1.f));

    {
        const auto b = title.getLocalBounds();
        title.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
        title.setPosition(cx, std::floor(center.y - 160.f));
        rt.draw(title);
    }
    sf::Text scoreT; scoreT.setFont(res_.font());
    scoreT.setCharacterSize(28);
    scoreT.setFillColor(sf::Color::White);
    scoreT.setString("Score: " + std::to_string(score_));
    {
        const auto b = scoreT.getLocalBounds();
        scoreT.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
        scoreT.setPosition(cx, std::floor(center.y - 120.f));
        rt.draw(scoreT);
    }
    
    auto list = hs_.topN((size_t)cfg_.popupRowsX);
    sf::Text hdr("Highlights (top " + std::to_string(cfg_.popupRowsX) + ")", res_.font(), 22);
    hdr.setFillColor(sf::Color(200, 200, 255));
    {
        const auto b = hdr.getLocalBounds();
        hdr.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
        hdr.setPosition(cx, std::floor(center.y - 80.f));
        rt.draw(hdr);
    }
    sf::Text row("", res_.font(), 20);
    const float listStartY = std::floor(center.y - 50.f);
    const float listStep = 24.f;
    for (size_t i = 0; i < list.size(); ++i) 
    {
        row.setString(std::to_string((int)i + 1) + ". " + list[i].name + " > " + std::to_string(list[i].score));
        row.setFillColor(sf::Color(120, 220, 120));
        const auto rb = row.getLocalBounds();
        row.setOrigin(rb.left + rb.width * 0.5f, rb.top + rb.height * 0.5f);
        row.setPosition(cx, std::floor(listStartY + (float)i * listStep));
        rt.draw(row);
    }

    if (phase_ == Phase::AskSave)
    {
        sf::Text q("Save result?", res_.font(), 24);
       {
        const auto b = q.getLocalBounds();
            q.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
            q.setPosition(cx, std::floor(listStartY + (float)list.size() * listStep + 30.f));
            q.setFillColor(sf::Color::White);
            rt.draw(q);
       }

        sf::Text a("No", res_.font(), 22);
        sf::Text b("Yes", res_.font(), 22);
        a.setFillColor(askSel_ == 0 ? sf::Color::Green : sf::Color::White);
        b.setFillColor(askSel_ == 1 ? sf::Color::Green : sf::Color::White);
       {
        const float y = std::floor(q.getPosition().y + 36.f);
            const auto ab = a.getLocalBounds();
            const auto bb = b.getLocalBounds();
            a.setOrigin(ab.left + ab.width * 0.5f, ab.top + ab.height * 0.5f);
            b.setOrigin(bb.left + bb.width * 0.5f, bb.top + bb.height * 0.5f);
            a.setPosition(std::floor(cx - 60.f), y);
            b.setPosition(std::floor(cx + 60.f), y);
            rt.draw(a); rt.draw(b);
       }

        sf::Text hint("W/S or Up/Down - move, Enter - select", res_.font(), 16);
       {
            const auto hb = hint.getLocalBounds();
            hint.setOrigin(hb.left + hb.width * 0.5f, hb.top + hb.height * 0.5f);
            hint.setPosition(cx, std::floor(listStartY + (float)list.size() * listStep + 100.f));
            hint.setFillColor(sf::Color(180, 180, 180));
            rt.draw(hint);
       }
        rt.setView(prev);
        return;
    }

    if (phase_ == Phase::EnterName)
    {
        sf::Text q("Type name (Enter - save, B/Esc - back):", res_.font(), 22);
        {
            const auto b = q.getLocalBounds();
            q.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
            q.setPosition(cx, std::floor(listStartY + (float)list.size() * listStep + 30.f));
            q.setFillColor(sf::Color::White);
            rt.draw(q);
        }
        sf::Text nameText(name_.empty() ? "ABC" : name_, res_.font(), 26);
        {
            const auto nb = nameText.getLocalBounds();
            nameText.setOrigin(nb.left + nb.width * 0.5f, nb.top + nb.height * 0.5f);
            nameText.setPosition(cx, std::floor(q.getPosition().y + 40.f));
            nameText.setFillColor(sf::Color::Yellow);
            rt.draw(nameText);
        }
        rt.setView(prev);
        return;
    }

    sf::Text o1("Start Game", res_.font(), 24);
    sf::Text o2("Main Menu", res_.font(), 24);
    o1.setFillColor(optSel_ == 0 ? sf::Color::Green : sf::Color::White);
    o2.setFillColor(optSel_ == 1 ? sf::Color::Green : sf::Color::White);
    {
        const auto b1 = o1.getLocalBounds();
        const auto b2 = o2.getLocalBounds();
        o1.setOrigin(b1.left + b1.width * 0.5f, b1.top + b1.height * 0.5f);
        o2.setOrigin(b2.left + b2.width * 0.5f, b2.top + b2.height * 0.5f);
        o1.setPosition(cx, std::floor(listStartY + (float)list.size() * listStep + 30.f));
        o2.setPosition(cx, std::floor(o1.getPosition().y + 40.f));
        rt.draw(o1); rt.draw(o2);
    }
    sf::Text hint("W/S or Up/Down - move, Enter - select", res_.font(), 16);
    {
        const auto hb = hint.getLocalBounds();
        hint.setOrigin(hb.left + hb.width * 0.5f, hb.top + hb.height * 0.5f);
        hint.setPosition(cx, std::floor(listStartY + (float)list.size() * listStep + 120.f));
        hint.setFillColor(sf::Color(180, 180, 180));
        rt.draw(hint);
    }
    rt.setView(prev);
}