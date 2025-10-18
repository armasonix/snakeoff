#pragma once
#include "Grid.h"
#include <string>

class Level 
{
public:
    explicit Level(Grid grid) : grid_(std::move(grid)) {}
    static Level loadFromFile(const std::string& path, int w, int h);
    const Grid& grid() const { return grid_; }
    Grid& grid() { return grid_; }
private:
    Grid grid_;
};