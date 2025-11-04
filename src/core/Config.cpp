#include "core/Config.h"
#include <fstream>
#include <string>
#include <sstream>

DifficultyParams Config::paramsFor(Difficulty d) const
{
    switch (d)
    {
        case Difficulty::D1: return { .stepSec = 0.16f, .pointsPerApple = 2,  .growthPerApple = 1 };
        case Difficulty::D2: return { .stepSec = 0.13f, .pointsPerApple = 4,  .growthPerApple = 1 };
        case Difficulty::D3: return { .stepSec = 0.10f, .pointsPerApple = 6,  .growthPerApple = 1 };
        case Difficulty::D4: return { .stepSec = 0.085f,.pointsPerApple = 8,  .growthPerApple = 1 };
        case Difficulty::D5: return { .stepSec = 0.070f,.pointsPerApple = 10, .growthPerApple = 1 };
    }
    return { .stepSec = 0.16f, .pointsPerApple = 2, .growthPerApple = 1 };
}

Config::Config() = default;

bool Config::loadUserSettings(const std::string& path)
{
    std::ifstream in(path);
    if (!in) return false;
    std::string line;
    while (std::getline(in, line))
    {
        std::istringstream is(line);
        std::string k, v;
        if (std::getline(is, k, '=') && std::getline(is, v))
        {
            if (k == "sound")       soundOn = (v == "1" || v == "true");
            if (k == "music")       musicOn = (v == "1" || v == "true");
            if (k == "windowScale") windowScale = std::max(0.5f, std::min(4.0f, std::stof(v)));
            if (k == "vsync")       vsync = (v == "1" || v == "true");
            if (k == "frameLimit")  frameLimit = std::max(0, std::stoi(v));
        }
    }
    return true;
}

bool Config::saveUserSettings(const std::string& path) const
{
    std::ofstream out(path, std::ios::trunc);
    if (!out) return false;
    out << "sound=" << (soundOn ? "1" : "0") << "\n";
    out << "music=" << (musicOn ? "1" : "0") << "\n";
    out << "windowScale=" << windowScale << "\n";
    out << "vsync=" << (vsync ? "1" : "0") << "\n";
    out << "frameLimit=" << frameLimit << "\n";
    return true;
}