#include "world/Level.h"
#include "world/CellType.h"
#include <algorithm>

Level Level::loadFromFile(const std::string& /*path*/, int w, int h)
{
    Grid g(w, h);
    g.fill(CellType::Empty);
    return Level(std::move(g));
}

int Level::cols() const noexcept { return grid_.w(); }
int Level::rows() const noexcept { return grid_.h(); }

bool Level::isBlocked(int x, int y) const noexcept
{
    if (x < 0 || y < 0 || x >= cols() || y >= rows()) return true;
    return grid_.get({ x, y }) == CellType::Wall;
}

void Level::clear()
{
    grid_.fill(CellType::Empty);
}

void Level::setCell(int x, int y, Cell c)
{
    grid_.set({ x, y }, c == Cell::Solid ? CellType::Wall : CellType::Empty);
}

void Level::buildBorders()
{
    grid_.buildBorders();
}

void Level::applyObstacles(const std::vector<sf::Vector2i>& blocks)
{
    for (auto& c : blocks)
    {
        if (c.x <= 0 || c.y <= 0 || c.x >= cols() - 1 || c.y >= rows() - 1) continue;
        setCell(c.x, c.y, Cell::Solid);
    }
}