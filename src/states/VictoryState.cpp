#include "states/VictoryState.h"
#include <cmath>

VictoryState::VictoryState(StateMachine& sm, sf::RenderWindow& win, Config& cfg, Resources& res,
    int levelIndex, int scoreValue, std::function<void()> onContinue)
    : sm_(sm), win_(win), cfg_(cfg), res_(res),
    levelIndex_(levelIndex), scoreValue_(scoreValue), onContinue_(std::move(onContinue))
{
    res_.switchToGameOver();
    // SFX win
    sfxWin_.setBuffer(res_.sfxWin());
    res_.playSfx(sfxWin_, 100.f);

    dim_.setSize({ (float)win_.getSize().x, (float)win_.getSize().y });
    dim_.setFillColor(sf::Color::Black);

    panel_.setSize({ 380.f, 220.f });
    panel_.setOrigin(panel_.getSize() * 0.5f);
    panel_.setFillColor(sf::Color::Black);
    panel_.setOutlineThickness(2.f);
    panel_.setOutlineColor(sf::Color::White);

    title_.setFont(res_.font());
    title_.setString(L"You Win!");
    title_.setCharacterSize(36);
    title_.setFillColor(sf::Color::White);

    btnSize_ = { 220.f, 46.f };
    btnNext_.setSize(btnSize_);
    btnMenu_.setSize(btnSize_);
    btnNext_.setOrigin(btnSize_ * 0.5f);
    btnMenu_.setOrigin(btnSize_ * 0.5f);

    for (auto* b : { &btnNext_, &btnMenu_ })
    {
        b->setFillColor(sf::Color::Black);
        b->setOutlineThickness(2.f);
        b->setOutlineColor(sf::Color(60, 60, 60));
    }

    btnNextText_.setFont(res_.font());
    btnNextText_.setString(L"Continue");
    btnNextText_.setCharacterSize(20);
    btnNextText_.setFillColor(sf::Color::White);

    btnMenuText_.setFont(res_.font());
    btnMenuText_.setString(L"Main Menu");
    btnMenuText_.setCharacterSize(20);
    btnMenuText_.setFillColor(sf::Color::White);
}

void VictoryState::handleEvent(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed) 
    {
        switch (e.key.code) 
        {
        case sf::Keyboard::Left:
        case sf::Keyboard::A:
        case sf::Keyboard::Up:
        case sf::Keyboard::W:
        case sf::Keyboard::Right:
        case sf::Keyboard::D:
        case sf::Keyboard::Down:
        case sf::Keyboard::S:
            sel_ ^= 1;
            break;

        case sf::Keyboard::Enter:
        case sf::Keyboard::Space: 
        {
            if (sel_ == 0) 
            {
                res_.ensureSessionLoop();
                if (onContinue_) onContinue_();
                sm_.pop();
            }
            else 
            {
                auto & sm = sm_;
                auto & win = win_;
                auto & cfg = cfg_;
                auto & res = res_;
                const int score = scoreValue_;
                sm_.pop();
                sm.push(std::make_unique<GameOverState>(sm, win, cfg, res, score));
                res.switchToGameOver();
            }
            break;
        }
        case sf::Keyboard::Escape: 
        {
            auto & sm = sm_;
            auto & win = win_;
            auto & cfg = cfg_;
            auto & res = res_;
            const int score = scoreValue_;
            sm_.pop();
            sm.push(std::make_unique<GameOverState>(sm, win, cfg, res, score));
            res.switchToGameOver();
            break;
        }
        default: break;
        }
    }
}

void VictoryState::update(float) {}

void VictoryState::draw(sf::RenderTarget& rt)
{
    auto prev = rt.getView();
    rt.setView(rt.getDefaultView());
    dim_.setSize(rt.getView().getSize());

    const sf::Vector2f center = win_.getView().getCenter();
    panel_.setPosition(center);
    title_.setPosition(center + sf::Vector2f(-70.f, -50.f));

    btnNext_.setPosition(center + sf::Vector2f(0.f, 20.f));
    btnMenu_.setPosition(center + sf::Vector2f(0.f, 80.f));
    centerTextOn(btnNext_, btnNextText_);
    centerTextOn(btnMenu_, btnMenuText_);

    const sf::Color kMenuGreen(120, 220, 120);
    btnNext_.setOutlineColor(sel_ == 0 ? sf::Color::White : sf::Color(60, 60, 60));
    btnMenu_.setOutlineColor(sel_ == 1 ? sf::Color::White : sf::Color(60, 60, 60));
    btnNext_.setFillColor(sel_ == 0 ? kMenuGreen : sf::Color::Black);
    btnMenu_.setFillColor(sel_ == 1 ? kMenuGreen : sf::Color::Black);
    btnNextText_.setStyle(sel_ == 0 ? sf::Text::Bold : sf::Text::Regular);
    btnMenuText_.setStyle(sel_ == 1 ? sf::Text::Bold : sf::Text::Regular);

    rt.draw(dim_);
    rt.draw(panel_);
    rt.draw(title_);
    rt.draw(btnNext_);
    rt.draw(btnMenu_);
    rt.draw(btnNextText_);
    rt.draw(btnMenuText_);

    rt.setView(prev);
}

void VictoryState::centerTextOn(const sf::RectangleShape& rect, sf::Text& txt) const
{
    const auto b = txt.getLocalBounds();
    txt.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
    const auto p = rect.getPosition();
    txt.setPosition(std::floor(p.x), std::floor(p.y));
}