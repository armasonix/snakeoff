#pragma once
#include <vector>
#include <random>
#include <SFML/System/Vector2.hpp>

struct ProcGenParams 
{
    int   maxPlacements = 24;   // max obstacle placements
    int   maxSingles = 10;   // max 1x1 obstacles
    int   safeRadius = 4;    // clear zone around snake spawn area
    float minReachable = 0.60f;// reachable free cells validation
    int   maxRetries = 20;   // number retries on validation fails
};

class ProcGen 
{
public:
    static std::vector<sf::Vector2i> generate(
        int cols, int rows, sf::Vector2i snakeStart,
        const ProcGenParams& p, std::mt19937& rng);
};