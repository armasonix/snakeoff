#pragma once
#include "util/Types.h"
#include <unordered_map>

class Config 
{
public:
    // globals
    float startDelaySec = 0.8f; // T
    int   popupRowsX = 5;       // X
    int   tableRowsY = 10;      // Y
    int   gridWidth = 32;
    int   gridHeight = 24;
    int   cellPx = 24;

    bool  soundOn = true;
    bool  musicOn = true;

    Difficulty difficulty = Difficulty::D1;

    DifficultyParams paramsFor(Difficulty d) const;
};