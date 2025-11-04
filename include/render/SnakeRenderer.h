#pragma once
#include <SFML/Graphics.hpp>
#include <array>
class Snake;
namespace render { struct SpriteRefs; }
namespace render { struct RenderStats; }

namespace render
{
    struct RenderStats;
    void drawSnakeBatched(sf::RenderTarget & world,
        const Snake & snake,
        int cellPx,
        const SpriteRefs & sprites,
        RenderStats * stats = nullptr);
}