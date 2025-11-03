#pragma once
#include <SFML/Graphics.hpp>

namespace render
{
    // save current view, set new in construct return old in destruct
    struct ScopedView
    {
        explicit ScopedView(sf::RenderTarget& rt, const sf::View& newView)
            : rt_(rt), prev_(rt.getView()) 
        {
            rt_.setView(newView);
        }
        // default view
        explicit ScopedView(sf::RenderTarget& rt)
            : rt_(rt), prev_(rt.getView()) 
        {
            rt_.setView(rt.getDefaultView());
        }
        ~ScopedView() { rt_.setView(prev_); }
    private:
        sf::RenderTarget& rt_;
        sf::View prev_;
    };
}