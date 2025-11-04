#pragma once
#include <SFML/Graphics.hpp>

class Level;

namespace render
{
    class GridBatch
    {
    public:
        void build(const Level& level, int cellPx,
            const sf::Texture& txWall,
            const sf::Texture& txObs);
        void markDirty() { dirty_ = true; }
        void rebuildIfDirty(const Level& level, int cellPx,
            const sf::Texture& txWall,
            const sf::Texture& txObs);
        void draw(sf::RenderTarget& rt) const;

        int verticesWall() const { return static_cast<int>(vaWall_.getVertexCount()); }
        int verticesObs()  const { return static_cast<int>(vaObs_.getVertexCount()); }
        int quadsWall() const { return verticesWall() / 4; }
        int quadsObs()  const { return verticesObs() / 4; }

    private:
        void rebuild_(const Level& level);

        bool dirty_{ true };
        int  cellPx_{ 0 };

        // border and obstacles batch
        sf::VertexArray vaWall_{ sf::Quads };
        sf::VertexArray vaObs_{ sf::Quads };

        const sf::Texture* txWall_{ nullptr };
        const sf::Texture* txObs_{ nullptr };
    };
}