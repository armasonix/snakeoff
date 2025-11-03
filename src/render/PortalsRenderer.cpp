#include "render/PortalsRenderer.h"
#include "core/Config.h"
#include "core/Resources.h"
#include "gfx/ShaderParams.h"

namespace
{
    inline sf::Vector2f cellCenter(int x, int y, int cellPx)
    {
        const float cs = static_cast<float>(cellPx);
        return { x * cs + cs * 0.5f, y * cs + cs * 0.5f };
    }
}

namespace render
{
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
        gfx::GlowParams gp;
        gp.strength = glowStrength;
        gp.haloScale = haloScale; 
        drawPortals(rt, cfg, res, portals, portalAnim, glow, glowReady, gp);
    }

    void drawPortals(sf::RenderTarget& rt,
        const Config& cfg,
        Resources& res,
        const std::vector<PortalPair>& portals,
        const std::vector<float>& portalAnim,
        sf::Shader* glow,
        bool glowReady,
        const gfx::GlowParams& baseGlow)
    {
        if (portals.empty()) return;

        for (size_t i = 0; i < portals.size(); ++i)
        {
            const float tAnim = portalAnim[i];
            const int frame = static_cast<int>(tAnim / 0.06f) % 8;

            const sf::Texture& tex = res.txPortal(frame);
            sf::Sprite s(tex);
            const auto ts = tex.getSize();

            s.setOrigin(ts.x * 0.5f, ts.y * 0.5f);
            s.setScale(cfg.cellPx / static_cast<float>(ts.x),
                cfg.cellPx / static_cast<float>(ts.y));

            auto drawOne = [&](const Vec2i& c)
                {
                    s.setPosition(cellCenter(c.x, c.y, cfg.cellPx));
                    rt.draw(s);

                    if (glowReady && glow)
                    {
                        gfx::GlowParams gp = baseGlow;
                        const auto col = portals[i].color;
                        gp.tint = sf::Glsl::Vec3(col.r / 255.f, col.g / 255.f, col.b / 255.f);
                        gp.texel = sf::Glsl::Vec2(1.f / ts.x, 1.f / ts.y);
                        gp.time = tAnim * 6.2831853f;
                        gp.apply(*glow);

                        sf::Sprite g = s;
                        g.setScale(s.getScale().x * gp.haloScale, s.getScale().y * gp.haloScale);

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