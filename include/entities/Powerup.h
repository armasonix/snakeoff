#pragma once
#include <cstdint>

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
};