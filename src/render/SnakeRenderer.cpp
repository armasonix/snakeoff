#include "render/SnakeRenderer.h"
#include "render/RenderStats.h"
#include "render/SpriteRefs.h"
#include "entities/Snake.h"
#include <cmath>
#include <algorithm>

namespace
{
    inline sf::Vector2f cellCenter(int x, int y, int cellPx)
    {
        const float cs = static_cast<float>(cellPx);
        return { x * cs + cs * 0.5f, y * cs + cs * 0.5f };
    }

    struct QuadWriter
    {
        sf::VertexArray va{ sf::Quads };
        const sf::Texture* tex = nullptr;
        void begin(const sf::Texture* t) { tex = t; va.clear(); }
        void addQuad(const sf::Vector2f& center, int cellPx,
            bool rotate90,
            float halfPixel = 0.5f)
        {
            const float w = cellPx * 0.5f, h = cellPx * 0.5f;
            const float x0 = center.x - w, y0 = center.y - h;
            const float x1 = center.x + w, y1 = center.y + h;

            sf::Vertex q[4];
            q[0].position = { x0, y0 };
            q[1].position = { x1, y0 };
            q[2].position = { x1, y1 };
            q[3].position = { x0, y1 };

            const auto ts = tex->getSize();
            const float u0 = halfPixel, v0 = halfPixel;
            const float u1 = ts.x - halfPixel, v1 = ts.y - halfPixel;

            if (!rotate90) 
            {
                q[0].texCoords = { u0, v0 };
                q[1].texCoords = { u1, v0 };
                q[2].texCoords = { u1, v1 };
                q[3].texCoords = { u0, v1 };
            }
            else 
            {
                q[0].texCoords = { u1, v0 };
                q[1].texCoords = { u1, v1 };
                q[2].texCoords = { u0, v1 };
                q[3].texCoords = { u0, v0 };
            }

            va.append(q[0]); va.append(q[1]); va.append(q[2]); va.append(q[3]);
        }
        void flush(sf::RenderTarget& rt)
        {
            if (!tex || va.getVertexCount() == 0) return;
            sf::RenderStates rs; rs.texture = tex;
            rt.draw(va, rs);
            va.clear();
        }
    };
}

namespace render
{
    void drawSnakeBatched(sf::RenderTarget& world,
        const Snake & snake,
        int cellPx,
        const SpriteRefs & sref,
        render::RenderStats * stats)
    {
        const auto& body = snake.body();
        if (body.empty()) return;
        int draws = 0;

        // head
        {
            const auto h = body.front();
            float rot = 0.f;
            if (body.size() >= 2)
            {
                const auto n = *(body.begin() + 1);
                const int dx = h.x - n.x, dy = h.y - n.y;
                if (dx == 1)       rot = 0.f;
                else if (dx == -1) rot = 180.f;
                else if (dy == 1)  rot = 90.f;
                else if (dy == -1) rot = 270.f;
            }
            sref.head.setRotation(rot);
            sref.head.setPosition(cellCenter(h.x, h.y, cellPx));
            world.draw(sref.head);
            ++draws;
        }

        // body and corners
        QuadWriter bodyH;     bodyH.begin(sref.body.getTexture());
        QuadWriter bodyV;     bodyV.begin(sref.body.getTexture());
        QuadWriter corner[4];
        for (int i = 0;i < 4;++i) corner[i].begin(sref.corners[i]->getTexture());

        auto addCorner = [&](int idx, const sf::Vector2f& pos) 
            {
            corner[idx].addQuad(pos, cellPx, false);
            };

        for (size_t i = 1; i + 1 < body.size(); ++i)
        {
            const auto prev = body[i - 1];
            const auto c = body[i];
            const auto next = body[i + 1];

            const int dx1 = c.x - prev.x, dy1 = c.y - prev.y;
            const int dx2 = next.x - c.x, dy2 = next.y - c.y;
            const bool straightH = (dy1 == 0 && dy2 == 0);
            const bool straightV = (dx1 == 0 && dx2 == 0);
            const bool cornerSeg = !(straightH || straightV);
            const sf::Vector2f pos = cellCenter(c.x, c.y, cellPx);

            if (!cornerSeg)
            {
                if (straightH) bodyH.addQuad(pos, cellPx, false);
                else           bodyV.addQuad(pos, cellPx, true);
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
                addCorner(idx, pos);
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

            const sf::Vector2f base = sref.tail.getScale();
            sf::Vector2f cur = base;
            if (rot == 0.f || rot == 180.f) cur.x *= 1.15f; else cur.y *= 1.15f;

            sref.tail.setRotation(rot);
            sref.tail.setScale(cur);
            sref.tail.setPosition(cellCenter(t.x, t.y, cellPx));
            world.draw(sref.tail);
            sref.tail.setScale(base);
            ++draws;
        }

        // flush
        auto tryFlush = [&](QuadWriter& w)
        {
            if (w.va.getVertexCount() > 0) { w.flush(world); ++draws; }
        };
        tryFlush(bodyH);
        tryFlush(bodyV);
        for (int i = 0; i < 4; ++i) tryFlush(corner[i]);
        if (stats) stats->addDraws(draws);
    }
}