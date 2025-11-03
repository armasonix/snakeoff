#pragma once
#include <SFML/Graphics.hpp>

namespace gfx
{
    struct ChromAbParams
    {
        sf::Glsl::Vec2 res{ 0.f, 0.f };
        float time{ 0.f };
        float amount{ 0.f };

        void apply(sf::Shader& shader) const
        {
            shader.setUniform("u_res", res);
            shader.setUniform("u_time", time);
            shader.setUniform("u_amount", amount);
        }
    };
}