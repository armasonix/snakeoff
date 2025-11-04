#pragma once
#include <SFML/Graphics.hpp>

namespace gfx
{
    struct ChromAbParams
    {
        sf::Glsl::Vec2 res{ 0.f,0.f };
        float time{ 0.f };
        float amount{ 0.f };
        void apply(sf::Shader& sh) const
        {
            sh.setUniform("u_res", res);
            sh.setUniform("u_time", time);
            sh.setUniform("u_amount", amount);
        }
    };

    struct GlowParams
    {
        sf::Glsl::Vec3 tint{ 1.f,1.f,1.f };
        sf::Glsl::Vec2 texel{ 1.f,1.f };
        float time{ 0.f };
        float strength{ 1.f };
        float haloScale{ 1.f };
        void apply(sf::Shader& sh) const
        {
            sh.setUniform("u_tint", tint);
            sh.setUniform("u_time", time);
            sh.setUniform("u_strength", strength);
            sh.setUniform("u_texel", texel);
        }
    };

    struct ConfuseParams
    {
        sf::Glsl::Vec2 res{ 0.f,0.f };
        float time{ 0.f };
        void apply(sf::Shader& sh) const
        {
            sh.setUniform("u_res", res);
            sh.setUniform("u_time", time);
        }
    };
}