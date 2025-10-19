#pragma once
#include "states/IGameState.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

class StateMachine;
class Resources;

class PauseState : public IGameState
{
public:
    PauseState(StateMachine& sm, Resources& res);

    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;

private:
    bool showHelp_ = true;

    StateMachine& sm_;
    Resources& res_;
};