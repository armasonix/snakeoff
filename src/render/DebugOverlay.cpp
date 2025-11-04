#include "render/DebugOverlay.h"
#include "core/Resources.h"
#include <string>
#include <cstdio>

namespace render
{
    void drawPerfOverlay(sf::RenderTarget& rt, Resources& res, const PerfData& pd)
    {
        static sf::Clock s_frameClock;
        static sf::Clock s_fpsClock;
        static int       s_fpsFrames = 0;
        static float     s_fps = 0.f;
        const float dtMs = s_frameClock.restart().asSeconds() * 1000.f;

        ++s_fpsFrames;
        const float elapsed = s_fpsClock.getElapsedTime().asSeconds();
        if (elapsed >= 0.5f)
        {
            s_fps = s_fpsFrames / elapsed;
            s_fpsFrames = 0;
            s_fpsClock.restart();
        }

        auto prev = rt.getView();
        rt.setView(rt.getDefaultView());

        sf::RectangleShape bg({ 300.f, 168.f });
        bg.setPosition(10.f, 10.f);
        bg.setFillColor(sf::Color(0, 0, 0, 140));
        bg.setOutlineColor(sf::Color(36, 200, 120, 200));
        bg.setOutlineThickness(1.f);

        sf::Text txt("", res.font(), 14);
        txt.setPosition(16.f, 16.f);
        txt.setFillColor(sf::Color(220, 255, 220));

        char buf[512];
        std::snprintf(buf, sizeof(buf),
            "FPS / frame:\n"
            "  fps    : %.1f\n"
            "  dt     : %.2f ms\n"
            "Perf (ms):\n"
            "  bg     : %.2f\n"
            "  grid   : %.2f  (wall %dq, obs %dq)\n"
            "  portals: %.2f\n"
            "  snake  : %.2f  (segments %d)\n"
            "  UI     : %.2f",
            s_fps, dtMs,
            pd.tBgMs,
            pd.tGridMs, pd.gridQuadsWall, pd.gridQuadsObs,
            pd.tPortalsMs,
            pd.tSnakeMs, pd.snakeSegments,
            pd.tUIMs);

        txt.setString(buf);

        rt.draw(bg);
        rt.draw(txt);
        rt.setView(prev);
    }
}