#pragma once
#include <cstdint>
#include <vector>
#include <deque>
#include <functional>

struct Vec2i { int x{ 0 }, y{ 0 }; };
inline bool operator==(const Vec2i& a, const Vec2i& b) { return a.x == b.x && a.y == b.y; }
inline bool operator!=(const Vec2i & a, const Vec2i & b) { return !(a == b); }
inline Vec2i operator+(const Vec2i& a, const Vec2i& b) { return { a.x + b.x, a.y + b.y }; }
inline Vec2i operator-(const Vec2i & a, const Vec2i & b) { return { a.x - b.x, a.y - b.y }; }

struct Vec2iHash
{
	std::size_t operator()(const Vec2i & v) const noexcept
	{
		return (static_cast<std::size_t>(static_cast<uint32_t>(v.x)) << 32)
		^ static_cast<std::size_t>(static_cast<uint32_t>(v.y));
	}
};