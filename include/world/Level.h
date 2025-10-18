#pragma once
#include "Grid.h"
#include <string>
#include <vector>
#include <cstdint> 
#include <SFML/Graphics.hpp>

enum class Cell : uint8_t { Empty = 0, Solid = 1 };

class Level 
{
public:
    explicit Level(Grid grid) : grid_(std::move(grid)) {}
    static Level loadFromFile(const std::string& path, int w, int h);
    const Grid& grid() const { return grid_; }
    Grid& grid() { return grid_; }

    int cols() const noexcept;
    int rows() const noexcept;
    bool isBlocked(int x, int y) const noexcept;      // solid border
    void clear();                                     // clear all cells
    void setCell(int x, int y, Cell c);

    void buildBorders();                              // walls on border
    void applyObstacles(const std::vector<sf::Vector2i>& blocks);
private:
    Grid grid_;
};