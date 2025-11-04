#include "world/Grid.h"
#include <algorithm>

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

void Grid::fill(CellType t) noexcept
{
    std::fill(data_.begin(), data_.end(), t);
}

void Grid::buildBorders() noexcept
{
    if (w_ <= 0 || h_ <= 0) return;
    for (int x = 0; x < w_; ++x)
    {
        set({ x, 0 }, CellType::Wall);
        set({ x, h_ - 1 }, CellType::Wall);
    }
    for (int y = 0; y < h_; ++y)
    {
        set({ 0,        y }, CellType::Wall);
        set({ w_ - 1,   y }, CellType::Wall);
    }
}