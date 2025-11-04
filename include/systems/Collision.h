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

    // suicide
    static bool headHitsSelf(const Snake& s)
    {
        const auto& body = s.body();
        if (body.empty()) return false;
        const auto h = body.front();
        for (size_t i = 1; i < body.size(); ++i)
            if (body[i] == h) return true;
        return false;
    }

    // block collision
    static bool cellBlocked(const Grid& g, const Vec2i& c)
    {
        if (!g.inside(c)) return true;
        const auto t = g.get(c);
        return t != CellType::Empty;
    }
};