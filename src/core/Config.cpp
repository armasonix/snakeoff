#include "core/Config.h"
DifficultyParams Config::paramsFor(Difficulty d) const 
{
    switch (d) 
    {
    case Difficulty::D1: return { .stepSec = 0.16f, .pointsPerApple = 2,  .growthPerApple = 1 };
    case Difficulty::D2: return { .stepSec = 0.13f, .pointsPerApple = 4,  .growthPerApple = 1 };
    case Difficulty::D3: return { .stepSec = 0.10f, .pointsPerApple = 6,  .growthPerApple = 1 };
    case Difficulty::D4: return { .stepSec = 0.085f,.pointsPerApple = 8,  .growthPerApple = 1 };
    case Difficulty::D5: return { .stepSec = 0.070f,.pointsPerApple = 10, .growthPerApple = 1 };
    }
    return { .stepSec = 0.16f, .pointsPerApple = 2, .growthPerApple = 1 };
}