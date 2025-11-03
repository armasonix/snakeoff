#include "render/UIRenderer.h"
#include "systems/Effects.h"
#include "systems/Score.h"
#include "entities/Snake.h"
#include "core/Resources.h"
#include "render/ScopedView.h"
#include <cmath>
#include <string>

namespace render
{
    void drawUI(sf::RenderTarget& rt, UIParams u)
    {
        ScopedView sv(rt); // default view scope

        // stage overlay
        if (u.stageTimer > 0.f)
        {
            const float kTotal = 2.0f;
            float t = std::max(0.f, std::min(u.stageTimer, kTotal)) / kTotal;
            sf::Color fill = u.stageText.getFillColor(); fill.a = (sf::Uint8)std::round(255.f * t);
            sf::Color out = u.stageText.getOutlineColor(); out.a = fill.a;
            u.stageText.setFillColor(fill);
            u.stageText.setOutlineColor(out);

            const auto center = rt.getDefaultView().getCenter();
            const auto b = u.stageText.getLocalBounds();
            u.stageText.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
            u.stageText.setPosition(std::floor(center.x), std::floor(center.y - 120.f));
            rt.draw(u.stageText);
        }

        // score
        {
            u.scoreRGB.setString("Score: " + std::to_string(u.score.value()));
            const float phase = u.scoreRGBElapsed * 2.5f * 6.2831853f;
            auto ch = [&](float p)->sf::Uint8
                {
                    float s = 0.5f + 0.5f * std::sin(phase + p);
                    return (sf::Uint8)std::round(255.f * s);
                };
            const sf::Uint8 R = ch(0.0f), G = ch(2.0943951f), B = ch(4.1887902f);
            u.scoreRGB.setFillColor(sf::Color(R, G, B, 255));
            u.scoreRGB.setOutlineColor(sf::Color(0, 0, 0, 220));
            const auto b = u.scoreRGB.getLocalBounds();
            u.scoreRGB.setOrigin(b.left + b.width, b.top);
            const auto dv = rt.getDefaultView();
            const float right = std::floor(dv.getCenter().x + dv.getSize().x * 0.5f);
            const float top = std::floor(dv.getCenter().y - dv.getSize().y * 0.5f);
            u.scoreRGB.setPosition(right - 12.f, top + 12.f);
            rt.draw(u.scoreRGB);
        }

        {
            static int prevScore = std::numeric_limits<int>::min();
            const int cur = u.score.value();
            if (cur != prevScore)
            {
                u.scoreRGB.setString("Score: " + std::to_string(cur));
                prevScore = cur;
            }
        }

        // speed / confuse info
        {
            sf::Text info("", u.res.font(), 16);
            info.setPosition(12.f, 48.f);
            std::string msg;
            if (u.effects.spdRemain() > 0.f)
                msg += "SPEED x" + std::to_string(u.effects.speedMul()) +
                " (" + std::to_string((int)std::ceil(u.effects.spdRemain())) + "s)  ";
            if (u.effects.invRemain() > 0.f)
                msg += "CONFUSE (" + std::to_string((int)std::ceil(u.effects.invRemain())) + "s)";
            info.setString(msg);
            info.setFillColor(sf::Color(180, 220, 180));
            rt.draw(info);
        }

        if (u.snake.breakerTimeLeft() > 0.f)
        {
            rt.draw(u.brBack);
            rt.draw(u.brFill);
            rt.draw(u.brText);
        }

        auto drawBanner = [&](sf::Text& t, float lifeT, float elapsed, float yOff)
            {
                if (!(lifeT > 0.f)) return;
                const float dur = 1.6f;
                const float tt = 1.0f - std::max(0.0f, std::min(lifeT / dur, 1.0f));

                const float phase = elapsed * 2.5f * 6.2831853f;
                auto ch = [&](float p)->sf::Uint8
                    {
                        float s = 0.5f + 0.5f * std::sin(phase + p);
                        return (sf::Uint8)std::round(255.f * s);
                    };
                const sf::Uint8 R = ch(0.0f), G = ch(2.0943951f), B = ch(4.1887902f);

                const float fade = std::sin(3.1415926f * tt);
                const sf::Uint8 A = (sf::Uint8)std::round(255.f * fade);

                sf::Color fill(R, G, B, A);
                sf::Color out = t.getOutlineColor(); out.a = A;
                t.setFillColor(fill);
                t.setOutlineColor(out);

                const auto c = rt.getDefaultView().getCenter();
                t.setPosition(std::floor(c.x), std::floor(c.y + yOff));
                rt.draw(t);
            };

        drawBanner(u.breakerBanner, u.breakerBannerT, u.breakerBannerElapsed, -100.f);
        drawBanner(u.turboBanner, u.turboBannerT, u.turboBannerElapsed, -60.f);
        drawBanner(u.poisonBanner, u.poisonBannerT, u.poisonBannerElapsed, -20.f);
    }
}