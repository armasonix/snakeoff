#pragma once
#include <cstdint>
struct Vec2i; 
enum class PowerUpKind : uint8_t
{
	Breaker = 0,
};

struct PowerUp
{
	int cellX = 0;
	int cellY = 0;
	PowerUpKind kind = PowerUpKind::Breaker;
	float ttl = 12.f;

	bool  tick(float dt);
	Vec2i cell() const;
	void  setCell(const Vec2i& c);
};