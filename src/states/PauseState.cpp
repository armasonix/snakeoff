#include "states/PauseState.h"
#include "core/StateMachine.h"
#include "core/Resources.h"
#include "states/PlayState.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

PauseState::PauseState(StateMachine& sm, Resources& res, PlayState* owner)
    : sm_(sm), res_(res), owner_(owner) {}

void PauseState::onEnter() 
{
    showHelp_ = true;
    res_.pauseMusic();
}

void PauseState::handleEvent(const sf::Event& e) 
{
    if (e.type == sf::Event::KeyPressed) 
    {
        switch (e.key.code) 
        {
        case sf::Keyboard::P:
        case sf::Keyboard::B:
            res_.resumeMusic();
            sm_.pop();
            return;
        case sf::Keyboard::Escape:
            res_.resumeMusic();
            sm_.pop();
            return;
        case sf::Keyboard::Enter:
            res_.resumeMusic();
            sm_.pop();
            return;
        case sf::Keyboard::H:
            showHelp_ = !showHelp_;
            return;
        default: break;
        }
    }
    return;
}

void PauseState::update(float) {}

void PauseState::draw(sf::RenderTarget& rt) 
{
    const auto view = rt.getView();
    const sf::Vector2f vs = view.getSize();
    const float W = vs.x;
    const float H = vs.y;

    sf::RectangleShape dim({ W, H });
    dim.setFillColor(sf::Color(0, 0, 0, 140));
    dim.setPosition(view.getCenter() - 0.5f * vs);
    rt.draw(dim);

    const float PW = std::min(560.f, W * 0.8f);
    const float PH = std::min(300.f, H * 0.6f);
    const sf::Vector2f panelPos = (view.getCenter() - 0.5f * sf::Vector2f{ PW, PH });

    sf::RectangleShape panel({ PW, PH });
    panel.setPosition(panelPos);
    panel.setFillColor(sf::Color(30, 30, 38, 230));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(70, 70, 90));
    rt.draw(panel);

    const sf::Font& font = res_.font();

    auto makeText = [&](const sf::String& s, unsigned sz, float x, float y, sf::Color c)
        {
        sf::Text t;
        t.setFont(font);
        t.setString(s);
        t.setCharacterSize(sz);
        t.setFillColor(c);
        t.setPosition(x, y);
        return t;
        };

    const float pad = 18.f;
    const float PX = panelPos.x;
    const float PY = panelPos.y;

    rt.draw(makeText(L"PAUSE", 28, PX + pad, PY + pad, sf::Color(200, 220, 255)));
    rt.draw(makeText(L"Enter / Esc / P - continue,  H - help", 18, PX + pad, PY + pad + 34.f,
        sf::Color(190, 200, 215)));

    if (showHelp_) 
    {
        float y = PY + pad + 70.f;
        const float xL = PX + pad + 12.f;
        const float sw = 16.f;

        auto row = [&](sf::Color swatch, const wchar_t* text) 
            {
            sf::RectangleShape rect({ sw, sw });
            rect.setPosition(xL, y + 5.f);
            rect.setFillColor(swatch);
            rt.draw(rect);

            sf::Text t = makeText(text, 18, xL + sw + 10.f, y, sf::Color(230, 235, 245));
            rt.draw(t);

            y += 26.f;
            };

        row(sf::Color(200, 200, 200), L"Regular apple +P scores, +L snake length");
        row(sf::Color(255, 215, 0), L"Bonus apple — x2 scores (TTL)");
        row(sf::Color(170, 80, 200), L"Poisoned apple — temporary acceleration (TTL)");
        row(sf::Color(80, 200, 200), L"Confusing apple — control inverse (TTL)");
        y += 6.f;
        row(sf::Color(120, 200, 255), L"Portals: enter - brighter, exit - darker");
    }
}