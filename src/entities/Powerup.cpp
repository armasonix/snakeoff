#include "entities/Powerup.h"
#include "util/Types.h"

bool PowerUp::tick(float dt)
{
	ttl -= dt;
	return ttl <= 0.f;
}

Vec2i PowerUp::cell() const
{
	return Vec2i{ cellX, cellY };
}

void PowerUp::setCell(const Vec2i & c)
{
	cellX = c.x;
	cellY = c.y;
}