#pragma once
#include <SFML/Graphics.hpp>
#include "render/SpriteRefs.h"

namespace render
{
    inline sf::Vector2f scaleToCell(const sf::Sprite& sp, int cellPx)
    {
        const auto sz = sp.getTexture() ? sp.getTexture()->getSize() : sf::Vector2u{ 1,1 };
        if (sz.x == 0 || sz.y == 0) return { 1.f,1.f };
        return { cellPx / static_cast<float>(sz.x), cellPx / static_cast<float>(sz.y) };
    }

    // scale main sprites for cell size
    inline void applyCellScale(SpriteRefs& r, int cellPx)
    {
        r.head.setScale(scaleToCell(r.head, cellPx));
        r.body.setScale(scaleToCell(r.body, cellPx));
        r.tail.setScale(scaleToCell(r.tail, cellPx));
        for (auto* c : r.corners) if (c) c->setScale(scaleToCell(*c, cellPx));

        r.apple1.setScale(scaleToCell(r.apple1, cellPx));
        r.apple2.setScale(scaleToCell(r.apple2, cellPx));
        r.apple3.setScale(scaleToCell(r.apple3, cellPx));
        r.powerBomb.setScale(scaleToCell(r.powerBomb, cellPx));
        r.powerMush.setScale(scaleToCell(r.powerMush, cellPx));

        r.explosion.setScale(scaleToCell(r.explosion, cellPx));
        r.wall.setScale(scaleToCell(r.wall, cellPx));
        r.obs.setScale(scaleToCell(r.obs, cellPx));
    }
}