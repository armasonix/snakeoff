#pragma once
#include "util/Types.h"
#include "CellType.h"
#include <vector>
#include <cassert>

class Grid
{
public:
    Grid(int w, int h) : w_(w), h_(h), data_(w* h, CellType::Empty) {}
    int w() const { return w_; }
    int h() const { return h_; }
    int area() const noexcept { return w_ * h_; }

    bool inside(const Vec2i& c) const { return c.x >= 0 && c.y >= 0 && c.x < w_ && c.y < h_; }

    CellType get(const Vec2i& c) const
    {
        assert(inside(c));
        return data_[c.y * w_ + c.x];
    }
    void set(const Vec2i& c, CellType t)
    {
        assert(inside(c));
        data_[c.y * w_ + c.x] = t;
    }

    void fill(CellType t) noexcept;
    void buildBorders() noexcept;

    bool isObstacle(int x, int y) const;
    bool destroyObstacle(int x, int y);

    bool isBorder(int x, int y) const noexcept
    {
        return (x == 0) || (y == 0) || (x == w() - 1) || (y == h() - 1);
    }

private:
    int w_, h_;
    std::vector<CellType> data_;
};