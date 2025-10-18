#include "util/Random.h"
int Random::range(int a, int b) 
{
    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> d(a, b);
    return d(rng);
}