#pragma once
#include <SFML/Graphics.hpp>
class Resources;
class Score;
class Effects;
class Snake;

namespace render
{
    struct UIParams
    {
        Resources& res;
        Score& score;
        Effects& effects;
        Snake& snake;

        sf::Text& stageText; float stageTimer;
        sf::Text& scoreRGB;  float scoreRGBElapsed;

        sf::RectangleShape& brBack;
        sf::RectangleShape& brFill;
        sf::Text& brText;

        sf::Text& breakerBanner; float breakerBannerT; float breakerBannerElapsed;
        sf::Text& turboBanner;   float turboBannerT;   float turboBannerElapsed;
        sf::Text& poisonBanner;  float poisonBannerT;  float poisonBannerElapsed;
    };
    void drawUI(sf::RenderTarget& rt, UIParams ui);
}