#pragma once
#include "states/IGameState.h"
#include "states/GameOverState.h"
#include "core/Config.h"
#include "core/Resources.h"
#include "core/StateMachine.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <functional>

class VictoryState : public IGameState
{
public:
	VictoryState(StateMachine & sm, sf::RenderWindow & win, Config & cfg, Resources & res,
	int levelIndex, int scoreValue, std::function<void()> onContinue);

	void handleEvent(const sf::Event & e) override;
	void update(float dt) override;
	void draw(sf::RenderTarget & rt) override;
	
private:
	StateMachine & sm_;
	sf::RenderWindow & win_;
	Config & cfg_;
	Resources & res_;
	int levelIndex_{ 1 };
	int scoreValue_{ 0 };
	std::function<void()> onContinue_;
	int sel_{ 0 };

	void centerTextOn(const sf::RectangleShape & rect, sf::Text & txt) const;
	
	sf::RectangleShape dim_, panel_, btnNext_, btnMenu_;
	sf::Vector2f btnSize_;
	sf::Text title_, btnNextText_, btnMenuText_;
	sf::Sound sfxWin_;
};