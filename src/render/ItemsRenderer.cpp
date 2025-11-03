#include "render/ItemsRenderer.h"
#include "core/Config.h"
#include "entities/Apple.h"

namespace
{
    inline sf::Vector2f cellCenter(int x, int y, int cellPx)
    {
        const float cs = static_cast<float>(cellPx);
        return { x * cs + cs * 0.5f, y * cs + cs * 0.5f };
    }

    inline sf::Color ttlColor(render::AppleKind k)
    {
        switch (k)
        {
            case render::AppleKind::Bonus:   return sf::Color(255, 215, 0);
            case render::AppleKind::Poison:  return sf::Color(170, 80, 200);
            case render::AppleKind::Confuse: return sf::Color(80, 200, 200);
            default: return sf::Color(200, 200, 200);
        }
    }

    inline float ttlTotalFor(const Config& cfg, render::AppleKind k)
    {
        switch (k)
        {
            case render::AppleKind::Bonus:   return cfg.apple.bonusTTL;
            case render::AppleKind::Poison:  return cfg.apple.poisonTTL;
            case render::AppleKind::Confuse: return cfg.apple.confuseTTL;
            default: return 0.f;
        }
    }
}

namespace render
{
    void drawItems(sf::RenderTarget& world,
        const Config& cfg,
        const Apple* apple,
        AppleKind    kind,
        float        appleTTL,
        const std::vector<PowerUp>& powerups,
        sf::Sprite& sprApple1,
        sf::Sprite& sprApple2,
        sf::Sprite& sprApple3,
        sf::Sprite& sprPowerBomb,
        sf::Sprite& sprPowerMush)
    {
        // apple
        if (apple)
        {
            const auto c = apple->cell();
            const auto pos = cellCenter(c.x, c.y, cfg.cellPx);
            sf::Sprite* s = &sprApple1;
            switch (kind)
            {
                case AppleKind::Bonus:   s = &sprApple2;   break;
                case AppleKind::Poison:  s = &sprApple3;   break;
                case AppleKind::Confuse: s = &sprPowerMush;break;
                default: break;
            }
            s->setRotation(0.f);
            s->setPosition(pos);
            world.draw(*s);

            // ttl bar
            const float ttlTotal = ttlTotalFor(cfg, kind);
            if (ttlTotal > 0.f && appleTTL > 0.f)
            {
                const float frac = std::clamp(appleTTL / ttlTotal, 0.f, 1.f);
                const float w = (cfg.cellPx - 2) * frac;

                sf::RectangleShape bar({ w, 4.f });
                bar.setPosition(std::floor(pos.x - cfg.cellPx * 0.5f + 1.f),
                    std::floor(pos.y - cfg.cellPx * 0.5f - 5.f));
                bar.setFillColor(ttlColor(kind));
                world.draw(bar);
            }
        }

        // powerups
        for (const auto& p : powerups)
        {
            const auto pos = cellCenter(p.cellX, p.cellY, cfg.cellPx);
            sf::Sprite* s = (p.kind == PowerUpKind::Breaker) ? &sprPowerBomb : &sprPowerMush;
            s->setRotation(0.f);
            s->setPosition(pos);
            world.draw(*s);
        }
    }
}