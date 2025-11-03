#include "render/SnakeRenderer.h"
#include "entities/Snake.h"

namespace render
{
    static inline sf::Vector2f cellCenter(int x, int y, int cellPx)
    {
        const float cs = static_cast<float>(cellPx);
        return { x * cs + cs * 0.5f, y * cs + cs * 0.5f };
    }

    void drawSnake(sf::RenderTarget& world,
        const Snake& snake,
        int cellPx,
        sf::Sprite& sprHead,
        sf::Sprite& sprBody,
        sf::Sprite& sprTail,
        const std::array<sf::Sprite*, 4>& sprBodyCorner)
    {
        const auto& body = snake.body();
        if (body.empty()) return;

        // head
        {
            auto h = body.front();
            float rot = 0.f;
            if (body.size() >= 2)
            {
                auto n = *(body.begin() + 1);
                const int dx = h.x - n.x, dy = h.y - n.y;
                if (dx == 1)       rot = 0.f;
                else if (dx == -1) rot = 180.f;
                else if (dy == 1)  rot = 90.f;
                else if (dy == -1) rot = 270.f;
            }
            sprHead.setRotation(rot);
            sprHead.setPosition(cellCenter(h.x, h.y, cellPx));
            world.draw(sprHead);
        }

        // body & corners
        for (size_t i = 1; i + 1 < body.size(); ++i)
        {
            const auto prev = body[i - 1];
            const auto c = body[i];
            const auto next = body[i + 1];

            const int dx1 = c.x - prev.x, dy1 = c.y - prev.y;
            const int dx2 = next.x - c.x, dy2 = next.y - c.y;
            const bool straightH = (dy1 == 0 && dy2 == 0);
            const bool straightV = (dx1 == 0 && dx2 == 0);
            const bool corner = !(straightH || straightV);
            const sf::Vector2f pos = cellCenter(c.x, c.y, cellPx);

            if (!corner)
            {
                sprBody.setRotation(straightV ? 90.f : 0.f);
                sprBody.setPosition(pos);
                world.draw(sprBody);
            }
            else
            {
                const bool hasLeft = (prev.x == c.x - 1) || (next.x == c.x - 1);
                const bool hasRight = (prev.x == c.x + 1) || (next.x == c.x + 1);
                const bool hasUp = (prev.y == c.y - 1) || (next.y == c.y - 1);
                const bool hasDown = (prev.y == c.y + 1) || (next.y == c.y + 1);
                int idx = 0;
                if (hasLeft && hasDown)       idx = 0;
                else if (hasLeft && hasUp)    idx = 1;
                else if (hasRight && hasDown) idx = 2;
                else                          idx = 3;

                auto& s = *sprBodyCorner[idx];
                s.setRotation(0.f);
                s.setPosition(pos);
                world.draw(s);
            }
        }

        // tail
        if (body.size() >= 2)
        {
            const auto t = body.back();
            const auto prev = *(body.end() - 2);
            const int dx = prev.x - t.x, dy = prev.y - t.y;
            float rot = 0.f;
            if (dx == 1)       rot = 0.f;
            else if (dx == -1) rot = 180.f;
            else if (dy == 1)  rot = 90.f;
            else if (dy == -1) rot = 270.f;
            rot += 180.f; if (rot >= 360.f) rot -= 360.f;

            const sf::Vector2f base = sprTail.getScale();
            sf::Vector2f cur = base;
            if (rot == 0.f || rot == 180.f) cur.x *= 1.15f; else cur.y *= 1.15f;

            sprTail.setRotation(rot);
            sprTail.setScale(cur);
            sprTail.setPosition(cellCenter(t.x, t.y, cellPx));
            world.draw(sprTail);
            sprTail.setScale(base);
        }
    }
}