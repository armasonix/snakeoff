#pragma once
#include "world/Grid.h"
#include "entities/Snake.h"

struct Collision 
{
    static bool headHitsWall(const Grid& g, const Snake& s) 
    {
        auto h = s.head();
        return !g.inside(h) || g.get(h) == CellType::Wall;
    }
};