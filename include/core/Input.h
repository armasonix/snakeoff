#pragma once
#include <SFML/Window.hpp>
#include <algorithm>
#include <vector>
#include <optional>

struct Input
{
    sf::Keyboard::Key up = sf::Keyboard::W;
    sf::Keyboard::Key down = sf::Keyboard::S;
    sf::Keyboard::Key left = sf::Keyboard::A;
    sf::Keyboard::Key right = sf::Keyboard::D;
    sf::Keyboard::Key enter = sf::Keyboard::Enter;
    sf::Keyboard::Key back = sf::Keyboard::B;
    sf::Keyboard::Key pause = sf::Keyboard::P;
};

enum class InputAction
{
    MoveUp, MoveDown, MoveLeft, MoveRight,
    Pause,
    TogglePerf, ToggleBackground, ToggleGrid, TogglePortals, ToggleItems,
    ToggleSnake, ToggleGate, ToggleConfuseFx, ToggleUI
};

struct InputResult
{
    std::vector<InputAction> actions;
};

inline InputResult mapKeyPressed(sf::Keyboard::Key key, const Input& map, bool inverted)
{
    InputResult r;

    auto isUp = (key == map.up) || (key == sf::Keyboard::Up);
    auto isDown = (key == map.down) || (key == sf::Keyboard::Down);
    auto isLeft = (key == map.left) || (key == sf::Keyboard::Left);
    auto isRight = (key == map.right) || (key == sf::Keyboard::Right);

    if (inverted)
    {
        std::swap(isUp, isDown);
        std::swap(isLeft, isRight);
    }

    if (isUp)    r.actions.push_back(InputAction::MoveUp);
    if (isDown)  r.actions.push_back(InputAction::MoveDown);
    if (isLeft)  r.actions.push_back(InputAction::MoveLeft);
    if (isRight) r.actions.push_back(InputAction::MoveRight);

    if (key == map.pause) r.actions.push_back(InputAction::Pause);

    // debug / render toggles
    switch (key)
    {
        case sf::Keyboard::F3:  r.actions.push_back(InputAction::TogglePerf);        break;
        case sf::Keyboard::F4:  r.actions.push_back(InputAction::ToggleBackground);  break;
        case sf::Keyboard::F5:  r.actions.push_back(InputAction::ToggleGrid);        break;
        case sf::Keyboard::F6:  r.actions.push_back(InputAction::TogglePortals);     break;
        case sf::Keyboard::F7:  r.actions.push_back(InputAction::ToggleItems);       break;
        case sf::Keyboard::F8:  r.actions.push_back(InputAction::ToggleSnake);       break;
        case sf::Keyboard::F9:  r.actions.push_back(InputAction::ToggleGate);        break;
        case sf::Keyboard::F10: r.actions.push_back(InputAction::ToggleConfuseFx);   break;
        case sf::Keyboard::F11: r.actions.push_back(InputAction::ToggleUI);          break;
        default: break;
    }

    return r;
}