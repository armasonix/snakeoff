#pragma once
#include "util/Types.h"
#include <unordered_map>

class Config 
{
public:
    Config();
    // globals
    float startDelaySec = 0.8f; // T
    int   popupRowsX = 5;       // X
    int   tableRowsY = 10;      // Y
    int   gridWidth = 32;
    int   gridHeight = 24;
    int   cellPx = 24;

    bool  soundOn = true;
    bool  musicOn = true;

    struct AppleParams 
    {
        // spawn weights
        int   wNormal = 70;
        int   wBonus = 15;
        int   wPoison = 10;
        int   wConfuse = 5;

        // bonus: score multi
        float bonusScoreMul = 2.0f;   // J+P*mul
        float bonusTTL = 6.0f;

        // poison: acceleration
        float poisonSpeedMul = 1.5f;   // speed multi
        float poisonDuration = 5.0f;
        float poisonTTL = 8.0f;

        // confuse: control inverse
        float confuseDuration = 4.0f;
        float confuseTTL = 8.0f;
    };

    AppleParams apple;

    Difficulty difficulty = Difficulty::D1;

    DifficultyParams paramsFor(Difficulty d) const;
};