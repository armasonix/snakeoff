#pragma once
#include <cstdint>
#include <vector>
#include <deque>

struct Vec2i { int x{ 0 }, y{ 0 }; };
inline bool operator==(const Vec2i& a, const Vec2i& b) { return a.x == b.x && a.y == b.y; }
inline Vec2i operator+(const Vec2i& a, const Vec2i& b) { return { a.x + b.x, a.y + b.y }; }

enum class Direction { Up, Down, Left, Right };

enum class Difficulty : int { D1 = 1, D2, D3, D4, D5 };

struct DifficultyParams 
{
    float stepSec;      // velocity: range between snake step updates
    int pointsPerApple; // P
    int growthPerApple; // L
};