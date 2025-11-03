#include "render/GridBatch.h"
#include "world/Level.h"

namespace
{
    inline void quad(sf::Vertex* v, float x, float y, float s,
        float u0, float v0, float u1, float v1)
    {
        v[0].position = { x,     y };
        v[1].position = { x + s, y };
        v[2].position = { x + s, y + s };
        v[3].position = { x,     y + s };

        v[0].texCoords = { u0, v0 };
        v[1].texCoords = { u1, v0 };
        v[2].texCoords = { u1, v1 };
        v[3].texCoords = { u0, v1 };
    }
}

namespace render
{
    void GridBatch::build(const Level& level, int cellPx,
        const sf::Texture& txWall,
        const sf::Texture& txObs)
    {
        cellPx_ = cellPx;
        txWall_ = &txWall;
        txObs_ = &txObs;
        dirty_ = true;
        rebuild_(level);
    }

    void GridBatch::rebuildIfDirty(const Level& level, int /*cellPx*/,
        const sf::Texture& /*txWall*/,
        const sf::Texture& /*txObs*/)
    {
        if (!dirty_) return;
        rebuild_(level);
    }

    void GridBatch::rebuild_(const Level& level)
    {
        dirty_ = false;

        const int W = level.grid().w();
        const int H = level.grid().h();
        const float s = static_cast<float>(cellPx_);

        // tex coords ñ half-pixel offset
        const auto wallSz = txWall_->getSize();
        const auto obsSz = txObs_->getSize();
        const float wu0 = 0.5f, wv0 = 0.5f, wu1 = wallSz.x - 0.5f, wv1 = wallSz.y - 0.5f;
        const float ou0 = 0.5f, ov0 = 0.5f, ou1 = obsSz.x - 0.5f, ov1 = obsSz.y - 0.5f;

        std::size_t walls = 0, obs = 0;
        vaWall_.resize(static_cast<std::size_t>(W * 2 + H * 2) * 4);
        vaObs_.resize(static_cast<std::size_t>(W * H) * 4);

        for (int y = 0; y < H; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                const float px = x * s, py = y * s;
                if (level.grid().isBorder(x, y))
                {
                    quad(&vaWall_[walls * 4], px, py, s, wu0, wv0, wu1, wv1);
                    ++walls;
                }
                else if (level.grid().isObstacle(x, y))
                {
                    quad(&vaObs_[obs * 4], px, py, s, ou0, ov0, ou1, ov1);
                    ++obs;
                }
            }
        }
        vaWall_.resize(walls * 4);
        vaObs_.resize(obs * 4);
    }

    void GridBatch::draw(sf::RenderTarget& rt) const
    {
        if (vaWall_.getVertexCount())
        {
            sf::RenderStates rs; rs.texture = txWall_;
            rt.draw(vaWall_, rs);
        }
        if (vaObs_.getVertexCount())
        {
            sf::RenderStates rs; rs.texture = txObs_;
            rt.draw(vaObs_, rs);
        }
    }
}