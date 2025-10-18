#pragma once
#include "core/State.h"
#include <vector>

class StateMachine 
{
public:
    void push(StatePtr s);
    void pop();
    IState* top();
private:
    std::vector<StatePtr> stack_;
};