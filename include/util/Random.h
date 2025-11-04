#pragma once
#include <random>

struct Random
{
    static int   range(int a, int b);
    static float rangef(float a, float b);
    static bool  chance(float p);
    static std::mt19937& rng();
};