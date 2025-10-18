#pragma once
#include "entities/Apple.h"

class NormalApple : public Apple 
{
public:
    using Apple::Apple;
    void onEaten(PlayContext& ctx) override;
};