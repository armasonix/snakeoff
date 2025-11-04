#include "render/GridRenderer.h"
#include "world/Level.h"

namespace render
{
    static inline sf::Vector2f cellCenter(int x, int y, int cellPx)
    {
        const float cs = static_cast<float>(cellPx);
        return { x * cs + cs * 0.5f, y * cs + cs * 0.5f };
    }

    void drawGrid(sf::RenderTarget& world,
        const Level& level,
        int cellPx,
        sf::Sprite& sprWall,
        sf::Sprite& sprObs)
    {
        const int W = level.grid().w();
        const int H = level.grid().h();

        for (int y = 0; y < H; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                const auto pos = cellCenter(x, y, cellPx);
                if (level.grid().isBorder(x, y))
                {
                    sprWall.setRotation(0.f);
                    sprWall.setPosition(pos);
                    world.draw(sprWall);
                }
                else if (level.grid().isObstacle(x, y))
                {
                    sprObs.setRotation(0.f);
                    sprObs.setPosition(pos);
                    world.draw(sprObs);
                }
            }
        }
    }
}