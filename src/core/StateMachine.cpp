#include "StateMachine.h"

void StateMachine::push(StatePtr s) { if (top()) top()->onExit(); stack_.push_back(std::move(s)); if (top()) top()->onEnter(); }
void StateMachine::pop() { if (top()) { stack_.back()->onExit(); stack_.pop_back(); if (top()) top()->onEnter(); } }

IState* StateMachine::top() { return stack_.empty() ? nullptr : stack_.back().get(); }