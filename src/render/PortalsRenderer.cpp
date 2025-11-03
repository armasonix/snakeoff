#include "render/PortalsRenderer.h"
#include "core/Config.h"
#include "core/Resources.h"

namespace render
{
    static inline sf::Vector2f cellCenter(int x, int y, int cellPx)
    {
        const float cs = static_cast<float>(cellPx);
        return { x * cs + cs * 0.5f, y * cs + cs * 0.5f };
    }

    void drawPortals(sf::RenderTarget& rt,
        const Config& cfg,
        Resources& res,
        const std::vector<PortalPair>& portals,
        const std::vector<float>& portalAnim,
        sf::Shader* glow,
        bool glowReady,
        float glowStrength,
        float haloScale,
        float glowPulseHz)
    {
        if (portals.empty()) return;

        const float cellW = static_cast<float>(cfg.cellPx);
        const float cellH = static_cast<float>(cfg.cellPx);

        for (size_t i = 0; i < portals.size(); ++i)
        {
            const float tAnim = portalAnim[i];
            const int frame = static_cast<int>(tAnim / 0.06f) % 8;

            const sf::Texture& tex = res.txPortal(frame);
            sf::Sprite s(tex);

            const auto ts = tex.getSize();
            s.setOrigin(ts.x * 0.5f, ts.y * 0.5f);
            s.setScale(cellW / ts.x, cellH / ts.y);

            const auto drawOne = [&](const Vec2i& c)
                {
                    s.setPosition(cellCenter(c.x, c.y, cfg.cellPx));
                    rt.draw(s);

                    if (glowReady && glow)
                    {
                        const auto col = portals[i].color;
                        glow->setUniform("u_tint", sf::Glsl::Vec3(col.r / 255.f, col.g / 255.f, col.b / 255.f));

                        const float phase = tAnim * glowPulseHz * 6.2831853f;
                        glow->setUniform("u_time", phase);
                        glow->setUniform("u_strength", glowStrength);
                        glow->setUniform("u_texel", sf::Glsl::Vec2(1.f / ts.x, 1.f / ts.y));

                        sf::Sprite g = s;
                        g.setScale(s.getScale().x * haloScale, s.getScale().y * haloScale);

                        sf::RenderStates rs;
                        rs.texture = &tex;
                        rs.shader = glow;
                        rs.blendMode = sf::BlendAdd;
                        rt.draw(g, rs);
                    }
                };

            drawOne(portals[i].a);
            drawOne(portals[i].b);
        }
    }
}