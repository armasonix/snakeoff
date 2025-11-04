#include "systems/Spawner.h"
#include "world/Grid.h"
#include "entities/Snake.h"
#include "entities/apples/NormalApple.h"
#include "util/Random.h"
#include <unordered_set>
#include <optional>
#include <iostream>

static bool occupiedBySnake(const Snake& s, const Vec2i& c)
{
    for (auto& b : s.body()) if (b == c) return true;
    return false;
}

std::optional<Vec2i> Spawner::tryRandomFreeCell(const Grid& g, const Snake& s)
{
    const int W = g.w(), H = g.h();
    if (W <= 0 || H <= 0) return std::nullopt;

    const int total = W * H;
    const int start = Random::range(0, total - 1);
    for (int i = 0; i < total; ++i)
    {
        const int idx = (start + i) % total;
        const Vec2i c{ idx % W, idx / W };
        if (g.get(c) == CellType::Empty && !occupiedBySnake(s, c))
            return c;
    }
    return std::nullopt;
}

Vec2i Spawner::randomFreeCell(const Grid& g, const Snake& s)
{
    if (auto c = tryRandomFreeCell(g, s)) return *c;
    std::cerr << "[Spawner] No free cells found; using fallback {1,1}\n";
    return { 1,1 };
}

std::unique_ptr<Apple> Spawner::spawnNormalTry(const Grid& g, const Snake& s)
{
    if (auto c = tryRandomFreeCell(g, s))
        return std::make_unique<NormalApple>(*c);
    return nullptr;
}

std::unique_ptr<Apple> Spawner::spawnNormal(const Grid& g, const Snake& s)
{
    if (auto a = spawnNormalTry(g, s)) return a;
    return std::make_unique<NormalApple>(Vec2i{ 1,1 });
}