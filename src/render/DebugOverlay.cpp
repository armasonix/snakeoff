#include "render/DebugOverlay.h"
#include "core/Resources.h"
#include <string>
#include <cstdio>

namespace render
{
    void drawPerfOverlay(sf::RenderTarget& rt, Resources& res, const PerfData& pd)
    {
        auto prev = rt.getView();
        rt.setView(rt.getDefaultView());

        sf::RectangleShape bg({ 280.f, 136.f });
        bg.setPosition(10.f, 10.f);
        bg.setFillColor(sf::Color(0, 0, 0, 140));
        bg.setOutlineColor(sf::Color(36, 200, 120, 200));
        bg.setOutlineThickness(1.f);

        sf::Text txt("", res.font(), 14);
        txt.setPosition(16.f, 16.f);
        txt.setFillColor(sf::Color(220, 255, 220));

        char buf[320];
        std::snprintf(buf, sizeof(buf),
            "Perf (ms):\n"
            "  bg     : %.2f\n"
            "  grid   : %.2f  (wall %dq, obs %dq)\n"
            "  portals: %.2f\n"
            "  snake  : %.2f  (segments %d)\n"
            "  UI     : %.2f",
            pd.tBgMs, pd.tGridMs, pd.gridQuadsWall, pd.gridQuadsObs,
            pd.tPortalsMs, pd.tSnakeMs, pd.snakeSegments, pd.tUIMs);

        txt.setString(buf);

        rt.draw(bg);
        rt.draw(txt);
        rt.setView(prev);
    }
}