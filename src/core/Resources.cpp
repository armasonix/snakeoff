#include "core/Resources.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

static bool ensureExists(const std::string& path, const char* kind) 
{
    if (!fs::exists(path)) 
    {
        std::cerr << "[RES] Missing " << kind << ": " << path << "\n"
            << "      cwd: " << fs::current_path().string() << "\n";
        return false;
    }
    return true;
}

bool Resources::load(const std::string& assetsDir)
{
    font_ = std::make_unique<sf::Font>();
    eat_ = std::make_unique<sf::SoundBuffer>();
    death_ = std::make_unique<sf::SoundBuffer>();

    const std::string fontPath = assetsDir + "/fonts/Roboto-Regular.ttf";
    const std::string eatPath = assetsDir + "/sfx/apple.wav";
    const std::string losePath = assetsDir + "/sfx/lose.wav";

    bool ok = true;

    ok &= ensureExists(fontPath, "font");
    ok &= ensureExists(eatPath, "sound");
    ok &= ensureExists(losePath, "sound");
    if (!ok) {
        std::cerr << "[RES] One or more asset files are missing. "
            << "Fix paths or copy assets next to the executable.\n";
        return false;
    }

    if (!font_->loadFromFile(fontPath))
    {
        std::cerr << "[RES] SFML failed to load font: " << fontPath << "\n";
        ok = false;
    }
    if (!eat_->loadFromFile(eatPath)) 
    {
        std::cerr << "[RES] SFML failed to load sound: " << eatPath << "\n";
        ok = false;
    }
    if (!death_->loadFromFile(losePath)) 
    {
        std::cerr << "[RES] SFML failed to load sound: " << losePath << "\n";
        ok = false;
    }

    if (!ok) 
    {
        std::cerr << "[RES] Resource loading FAILED. "
            << "Check file formats/permissions and that the files are valid.\n";
    }
    return ok;
}