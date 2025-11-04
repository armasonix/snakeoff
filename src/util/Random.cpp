#include "util/Random.h"
#include <algorithm>
#include <random>
#include <limits>
#include <cmath>

static std::mt19937& tl_rng()
{
    static thread_local std::mt19937 g{ std::random_device{}() };
    return g;
}

int Random::range(int a, int b)
{
    if (a > b) std::swap(a, b);
    std::uniform_int_distribution<int> d(a, b);
    return d(tl_rng());
}

float Random::rangef(float a, float b)
{
    if (a > b) std::swap(a, b);
    std::uniform_real_distribution<float> d(a, std::nextafter(b, std::numeric_limits<float>::max()));
    return d(tl_rng());
}

bool Random::chance(float p)
{
    p = std::clamp(p, 0.0f, 1.0f);
    std::bernoulli_distribution d(p);
    return d(tl_rng());
}

std::mt19937& Random::rng()
{
    return tl_rng();
}