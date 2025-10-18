#pragma once
#include <memory>

struct IState 
{
    virtual ~IState() = default;
    virtual void onEnter() {}
    virtual void onExit() {}
};

using StatePtr = std::unique_ptr<IState>;