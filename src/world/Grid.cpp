#include "world/Grid.h"


bool Grid::isObstacle(int x, int y) const 
{
    const Vec2i c{ x,y };
    return inside(c) && (get(c) == CellType::Wall);
}

bool Grid::destroyObstacle(int x, int y) 
{
    const Vec2i c{ x,y };
    if (!inside(c)) return false;
    if (get(c) != CellType::Wall) return false;
    set(c, CellType::Empty);
    return true;
}