#pragma once
#include <SFML/Graphics.hpp>

class Resources;

namespace render
{
    struct PerfData
    {
        float tBgMs = 0.f;
        float tGridMs = 0.f;
        float tPortalsMs = 0.f;
        float tSnakeMs = 0.f;
        float tUIMs = 0.f;

        int gridQuadsWall = 0;
        int gridQuadsObs = 0;
        int snakeSegments = 0;
    };

    // draw perf metrics table
    void drawPerfOverlay(sf::RenderTarget& rt, Resources& res, const PerfData& pd);
}