#include "gfx/Composite.h"

namespace gfx
{
    void drawChromAbComposite(sf::RenderTarget& rt,
        sf::RenderTexture& src,
        sf::Shader& shader,
        const sf::Vector2f& resPx,
        float time,
        float amount,
        const sf::View& worldView,
        const sf::Vector2f& cachedRes,
        float cachedAmount,
        sf::Vector2f* outCachedRes,
        float* outCachedAmount)
    {
        src.display();
        sf::Sprite full(src.getTexture());

        if (resPx != cachedRes) 
        {
            shader.setUniform("u_res", sf::Glsl::Vec2(resPx.x, resPx.y));
            if (outCachedRes) *outCachedRes = resPx;
        }
        shader.setUniform("u_time", time);
        if (amount != cachedAmount) 
        {
            shader.setUniform("u_amount", amount);
            if (outCachedAmount) *outCachedAmount = amount;
        }

        sf::RenderStates rs; rs.shader = &shader;
        auto prev = rt.getView();
        rt.setView(worldView);
        rt.draw(full, rs);
        rt.setView(prev);
    }
}