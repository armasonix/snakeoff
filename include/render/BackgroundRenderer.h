#pragma once
#include <SFML/Graphics.hpp>

namespace render
{
    // if shader != nullptr apply desaturate
    inline void drawBackground(sf::RenderTarget& rt,
        const sf::VertexArray& vaGround,
        const sf::Texture& txGround,
        sf::Shader* shader)
    {
        if (!vaGround.getVertexCount()) return;
        sf::RenderStates rs;
        rs.texture = &txGround;
        if (shader) rs.shader = shader;
        rt.draw(vaGround, rs);
    }
}