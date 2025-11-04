#pragma once
#include "states/IGameState.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Audio.hpp>

class StateMachine;
class Resources;
class PlayState;

class PauseState : public IGameState
{
public:
    PauseState(StateMachine& sm, Resources& res, PlayState* owner = nullptr);

    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& rt) override;

    void setOwner(PlayState* owner) { owner_ = owner; }

private:
    bool showHelp_ = true;

    StateMachine& sm_;
    Resources& res_;
    PlayState* owner_ = nullptr;

    sf::Sound sfxMove_;
    sf::Sound sfxHit_;
};