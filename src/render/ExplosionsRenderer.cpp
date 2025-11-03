#include "render/ExplosionsRenderer.h"
#include <algorithm>
#include <cmath>

namespace render
{
    void drawExplosions(sf::RenderTarget& world,
        const sf::Sprite& sprExpl,
        const std::vector<sf::Vector2f>& positions,
        const std::vector<float>& times,
        float lifeSeconds)
    {
        const sf::Texture* tx = sprExpl.getTexture();
        if (!tx) return;
        const std::size_t N = std::min(positions.size(), times.size());
        if (N == 0) return;

        const auto ts = tx->getSize();
        const float hw = ts.x * 0.5f;
        const float hh = ts.y * 0.5f;

        sf::VertexArray va(sf::Quads);
        va.resize(N * 4);

        const float u0 = 0.5f, v0 = 0.5f;
        const float u1 = ts.x - 0.5f, v1 = ts.y - 0.5f;

        for (std::size_t i = 0; i < N; ++i)
        {
            const sf::Vector2f& pos = positions[i];
            const float t = times[i];

            const float tNorm = 1.0f - std::clamp(t / lifeSeconds, 0.0f, 1.0f);
            const float sc = 0.90f + 1.10f * tNorm;
            const float w = hw * sc;
            const float h = hh * sc;

            const float aLin = std::max(0.0f, 1.0f - tNorm);
            const sf::Uint8 A = static_cast<sf::Uint8>(255.0f * std::sqrt(aLin));

            sf::Vertex* q = &va[i * 4];

            const float x0 = pos.x - w;
            const float y0 = pos.y - h;
            const float x1 = pos.x + w;
            const float y1 = pos.y + h;

            q[0].position = { x0, y0 };
            q[1].position = { x1, y0 };
            q[2].position = { x1, y1 };
            q[3].position = { x0, y1 };

            q[0].texCoords = { u0, v0 };
            q[1].texCoords = { u1, v0 };
            q[2].texCoords = { u1, v1 };
            q[3].texCoords = { u0, v1 };

            q[0].color = q[1].color = q[2].color = q[3].color = sf::Color(255, 255, 255, A);
        }

        sf::RenderStates rs;
        rs.texture = tx;
        rs.blendMode = sf::BlendAdd;

        world.draw(va, rs);
    }
}