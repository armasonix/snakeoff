#include "world/Level.h"
#include "world/CellType.h"
#include <algorithm>

Level Level::loadFromFile(const std::string& /*path*/, int w, int h)
{
    Grid g(w, h);

    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            g.set({ x, y }, CellType::Empty);

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
    for (int y = 0; y < rows(); ++y)
        for (int x = 0; x < cols(); ++x)
            grid_.set({ x, y }, CellType::Empty);
}

void Level::setCell(int x, int y, Cell c)
{
    grid_.set({ x, y }, c == Cell::Solid ? CellType::Wall : CellType::Empty);
}

void Level::buildBorders()
{

    for (int x = 0; x < cols(); ++x) 
    {
        setCell(x, 0, Cell::Solid);
        setCell(x, rows() - 1, Cell::Solid);
    }

    for (int y = 0; y < rows(); ++y) 
    {
        setCell(0, y, Cell::Solid);
        setCell(cols() - 1, y, Cell::Solid);
    }
}

void Level::applyObstacles(const std::vector<sf::Vector2i>& blocks)
{

    for (auto& c : blocks) 
    {
        if (c.x <= 0 || c.y <= 0 || c.x >= cols() - 1 || c.y >= rows() - 1) continue;
        setCell(c.x, c.y, Cell::Solid);
    }
}