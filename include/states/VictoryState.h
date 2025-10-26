#pragma once
#include "states/IGameState.h"
#include "states/GameOverState.h"
#include "core/Config.h"
#include "core/Resources.h"
#include "core/StateMachine.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>

class VictoryState : public IGameState 
{
public:
	VictoryState(StateMachine & sm, sf::RenderWindow & win, Config & cfg, Resources & res,
		int levelIndex, int scoreValue, std::function<void()> onContinue)
		 : sm_(sm), win_(win), cfg_(cfg), res_(res),
		levelIndex_(levelIndex), scoreValue_(scoreValue), onContinue_(std::move(onContinue)) 
	{
			dim_.setSize({ (float)win_.getSize().x, (float)win_.getSize().y });
		dim_.setFillColor(sf::Color::Black);
			panel_.setSize({ 380.f, 220.f });
		panel_.setOrigin(panel_.getSize() * 0.5f);
		panel_.setFillColor(sf::Color::Black);
		panel_.setOutlineThickness(2.f);
		panel_.setOutlineColor(sf::Color::White);
		title_.setFont(res_.font());
		title_.setString(L"Победа!");
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

		void handleEvent(const sf::Event & e) override 
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
				if (sel_ == 0) 
				{
					if (onContinue_) onContinue_();
					sm_.pop();
				}
				else 
				{
					sm_.push(std::make_unique<GameOverState>(sm_, win_, cfg_, res_, scoreValue_));
					res_.switchToGameOver();
				}
				break;
			case sf::Keyboard::Escape:
				sm_.push(std::make_unique<GameOverState>(sm_, win_, cfg_, res_, scoreValue_));
				res_.switchToGameOver();
				break;
			default: break;
			}
		}
	}
		void update(float) override { /* static menu */ }
		void draw(sf::RenderTarget & rt) override 
		{
		auto prev = rt.getView();
		rt.setView(rt.getDefaultView());
		const sf::Vector2f center = win_.getView().getCenter();
		panel_.setPosition(center);
		title_.setPosition(center + sf::Vector2f(-70.f, -50.f));
		btnNext_.setPosition(center + sf::Vector2f(0.f, 20.f));
		btnMenu_.setPosition(center + sf::Vector2f(0.f, 80.f));
		btnNextText_.setPosition(btnNext_.getPosition() + sf::Vector2f(-70.f, -14.f));
		btnMenuText_.setPosition(btnMenu_.getPosition() + sf::Vector2f(-35.f, -14.f));

		btnNext_.setOutlineColor(sel_ == 0 ? sf::Color::White : sf::Color(60, 60, 60));
		btnMenu_.setOutlineColor(sel_ == 1 ? sf::Color::White : sf::Color(60, 60, 60));
		btnNextText_.setStyle(sel_ == 0 ? sf::Text::Bold : sf::Text::Regular);
		btnMenuText_.setStyle(sel_ == 1 ? sf::Text::Bold : sf::Text::Regular);
		// render
		rt.draw(dim_);
		rt.draw(panel_);
		rt.draw(title_);
		rt.draw(btnNext_);
		rt.draw(btnMenu_);
		rt.draw(btnNextText_);
		rt.draw(btnMenuText_);
		rt.setView(prev);
	}

private:
	StateMachine & sm_;
	sf::RenderWindow & win_;
	Config & cfg_;
	Resources & res_;
	int levelIndex_{ 1 };
	int scoreValue_{ 0 };
	std::function<void()> onContinue_;
	int sel_{ 0 };
	sf::RectangleShape dim_, panel_, btnNext_, btnMenu_;
	sf::Vector2f btnSize_;
	sf::Text title_, btnNextText_, btnMenuText_;
};