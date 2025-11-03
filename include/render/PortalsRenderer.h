#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "world/Portal.h"
class Config;
class Resources;
namespace gfx { struct GlowParams; }

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
        float glowPulseHz);

    void drawPortals(sf::RenderTarget& rt,
        const Config& cfg,
        Resources& res,
        const std::vector<PortalPair>& portals,
        const std::vector<float>& portalAnim,
        sf::Shader* glow,
        bool glowReady,
        const gfx::GlowParams& baseGlow);
}