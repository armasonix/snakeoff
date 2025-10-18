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

    bool inside(const Vec2i& c) const { return c.x >= 0 && c.y >= 0 && c.x < w_ && c.y < h_; }
    CellType get(const Vec2i& c) const { return data_[c.y * w_ + c.x]; }
    void set(const Vec2i& c, CellType t) { data_[c.y * w_ + c.x] = t; }

private:
    int w_, h_;
    std::vector<CellType> data_;
};