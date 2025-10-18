#include "systems/Spawner.h"
#include "world/Grid.h"
#include "entities/Snake.h"
#include "entities/apples/NormalApple.h"
#include "util/Random.h"
#include <unordered_set>

static bool occupiedBySnake(const Snake& s, const Vec2i& c) 
{
    for (auto& b : s.body()) if (b == c) return true;
    return false;
}

Vec2i Spawner::randomFreeCell(const Grid& g, const Snake& s) 
{
    // simple cycle with random start
    const int W = g.w(), H = g.h();
    int start = Random::range(0, W * H - 1);
    for (int i = 0;i < W * H;++i) 
    {
        int idx = (start + i) % (W * H);
        Vec2i c{ idx % W, idx / W };
        if (g.get(c) == CellType::Empty && !occupiedBySnake(s, c)) return c;
    }
    return { 1,1 };
}

std::unique_ptr<Apple> Spawner::spawnNormal(const Grid& g, const Snake& s) 
{
    return std::make_unique<NormalApple>(randomFreeCell(g, s));
}