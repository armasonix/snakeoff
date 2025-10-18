#pragma once
#include "util/Types.h"
#include <memory>
#include <vector>

class Grid;
class Snake;
class Apple;

class Spawner 
{
public:
    // find free cell
    static Vec2i randomFreeCell(const Grid& g, const Snake& s);

    static std::unique_ptr<Apple> spawnNormal(const Grid& g, const Snake& s);
};