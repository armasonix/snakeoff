#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "entities/Powerup.h"

class Config;
class Apple;
namespace render { struct SpriteRefs; }
namespace render { struct RenderStats; }

namespace render
{
    enum class AppleKind { Normal, Bonus, Poison, Confuse };

    struct RenderStats;
    void drawItems(sf::RenderTarget & world,
        const Config & cfg,
        const Apple * apple,
        AppleKind    kind,
        float        appleTTL,
        const std::vector<PowerUp>&powerups,
        const SpriteRefs & sprites,
        RenderStats * stats = nullptr);
}