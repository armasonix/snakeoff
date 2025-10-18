#include "world/ProcGen.h"
#include <queue>
#include <array>
#include <algorithm>
#include <cmath>

static bool inside(int x, int y, int cols, int rows) { return x >= 0 && y >= 0 && x < cols && y < rows; }
static int  idx(int x, int y, int cols) { return y * cols + x; }
static bool inSafeZone(int x, int y, sf::Vector2i start, int r) 
{
    return (std::abs(x - start.x) <= r) && (std::abs(y - start.y) <= r);
}

static float reachableFraction(const std::vector<uint8_t>& occ, int cols, int rows, sf::Vector2i start)
{
    if (!inside(start.x, start.y, cols, rows)) return 0.f;
    if (occ[idx(start.x, start.y, cols)])     return 0.f;

    std::vector<uint8_t> vis(occ.size(), 0);
    std::queue<sf::Vector2i> q;
    q.push(start); vis[idx(start.x, start.y, cols)] = 1;

    int freeTotal = 0, freeReach = 0;
    for (int y = 0;y < rows;y++)
        for (int x = 0;x < cols;x++)
            if (!occ[idx(x, y, cols)]) freeTotal++;

    const int DX[4] = { 1,-1,0,0 }, DY[4] = { 0,0,1,-1 };
    while (!q.empty()) 
    {
        auto c = q.front(); q.pop();
        freeReach++;
        for (int k = 0;k < 4;k++) 
        {
            int nx = c.x + DX[k], ny = c.y + DY[k];
            if (!inside(nx, ny, cols, rows)) continue;
            int id = idx(nx, ny, cols);
            if (occ[id] || vis[id]) continue;
            vis[id] = 1; q.push({ nx,ny });
        }
    }
    return (freeTotal == 0) ? 0.f : (float)freeReach / (float)freeTotal;
}

// patterns: 1x1, 2x1, 3x1, 2x2
struct Pattern { std::vector<sf::Vector2i> cells; };
static const std::array<Pattern, 4> PATTERNS = 
{ 
    {
    {{ { {0,0} } }},                                  // 1x1
    {{ { {0,0}, {1,0} } }},                           // 2x1
    {{ { {0,0}, {1,0}, {2,0} } }},                    // 3x1
    {{ { {0,0}, {1,0}, {0,1}, {1,1} } }},             // 2x2
} };

std::vector<sf::Vector2i> ProcGen::generate(
    int cols, int rows, sf::Vector2i start,
    const ProcGenParams& p, std::mt19937& rng)
{
    std::uniform_int_distribution<int> patDist(0, (int)PATTERNS.size() - 1);

    for (int attempt = 0; attempt < p.maxRetries; ++attempt) 
    {
        std::vector<uint8_t> occ(cols * rows, 0);

        for (int y = 1; y < rows - 1; ++y)
            for (int x = 1; x < cols - 1; ++x)
                if (inSafeZone(x, y, start, p.safeRadius))
                    occ[idx(x, y, cols)] = 0;

        int placed = 0, singles = 0;
        int guard = p.maxPlacements * 20;

        while (placed < p.maxPlacements && guard-- > 0) 
        {
            int pick = patDist(rng);
            if (pick == 0 && singles >= p.maxSingles) continue;

            const auto& pat = PATTERNS[pick];

            int maxW = 0, maxH = 0;
            for (auto& c : pat.cells) { maxW = std::max(maxW, c.x); maxH = std::max(maxH, c.y); }
            if (cols < 3 || rows < 3) break;

            std::uniform_int_distribution<int> xdist(1, (cols - 2) - maxW);
            std::uniform_int_distribution<int> ydist(1, (rows - 2) - maxH);
            int x0 = xdist(rng), y0 = ydist(rng);

            bool ok = true;
            for (auto& c : pat.cells) 
            {
                int x = x0 + c.x, y = y0 + c.y;
                if (!inside(x, y, cols, rows)) { ok = false; break; }
                if (inSafeZone(x, y, start, p.safeRadius)) { ok = false; break; }
                if (occ[idx(x, y, cols)]) { ok = false; break; }
            }
            if (!ok) continue;

            for (auto& c : pat.cells) 
            {
                int x = x0 + c.x, y = y0 + c.y;
                occ[idx(x, y, cols)] = 1;
            }
            placed++;
            if (pick == 0) singles++;
        }

        // validation
        float frac = reachableFraction(occ, cols, rows, start);
        if (frac >= p.minReachable) 
        {
            std::vector<sf::Vector2i> out;
            out.reserve(placed * 2);
            for (int y = 1; y < rows - 1; ++y)
                for (int x = 1; x < cols - 1; ++x)
                    if (occ[idx(x, y, cols)]) out.push_back({ x,y });
            return out;
        }
    }
    return {};
}