#pragma once
#include "util/Types.h"
#include <memory>
#include <vector>
#include <optional>

class Grid;
class Snake;
class Apple;

class Spawner
{
public:
    // find free cell
    static Vec2i randomFreeCell(const Grid& g, const Snake& s);

    static std::optional<Vec2i> tryRandomFreeCell(const Grid& g, const Snake& s);
    static std::unique_ptr<Apple> spawnNormal(const Grid& g, const Snake& s);
    static std::unique_ptr<Apple> spawnNormalTry(const Grid& g, const Snake& s);
};