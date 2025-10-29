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

void Resources::setSoundEnabled(bool on) 
{
    soundOn_ = on;
}

void Resources::setMusicEnabled(bool on)
{
    musicOn_ = on;
    if (!on) 
    {
        if (music_.getStatus() == sf::Music::Playing) music_.pause();
    }
    else 
    {
        if (music_.getStatus() == sf::Music::Paused) music_.play();
    }
}

bool Resources::openAndPlay_(const std::string& path, bool loop, float vol, MusicTrack t)
{
    if (!musicOn_) 
    {
        currentTrack_ = t;
        currentPath_ = path;
        return true;
    }

    if (music_.getStatus() == sf::Music::Playing && currentPath_ == path)
        return true;

    if (music_.getStatus() == sf::Music::Paused && currentPath_ == path) 
    {
        music_.play();
        currentTrack_ = t;
        return true;
    }

    music_.stop();
    if (!music_.openFromFile(path))
        return false;

    music_.setLoop(loop);
    music_.setVolume(vol);
    music_.play();

    currentTrack_ = t;
    currentPath_ = path;
    return true;
}

void Resources::ensureMenuLoop(float vol)
{
    openAndPlay_("assets/music/mainMenu.ogg", true, vol, MusicTrack::Menu);
}

void Resources::ensureSessionLoop(float vol)
{
    openAndPlay_("assets/music/theme.ogg", true, vol, MusicTrack::Session);
}

void Resources::switchToGameOver(float vol)
{
    music_.stop();
    openAndPlay_("assets/music/game_over.wav", false, vol, MusicTrack::GameOver);
}

void Resources::pauseMusic()
{
    if (music_.getStatus() == sf::Music::Playing)
        music_.pause();
}

void Resources::resumeMusic()
{
    if (!musicOn_) return;
    if (music_.getStatus() == sf::Music::Paused)
        music_.play();
}

bool Resources::load(const std::string& assetsDir)
{
    font_ = std::make_unique<sf::Font>();
    eat_ = std::make_unique<sf::SoundBuffer>();
    death_ = std::make_unique<sf::SoundBuffer>();
    win_ = std::make_unique<sf::SoundBuffer>();
    bonus_ = std::make_unique<sf::SoundBuffer>();
    break_ = std::make_unique<sf::SoundBuffer>();
    portal_ = std::make_unique<sf::SoundBuffer>();
    sfxUiHitBuf_ = std::make_unique<sf::SoundBuffer>();
    sfxUiMoveBuf_ = std::make_unique<sf::SoundBuffer>();

    const std::string fontPath = assetsDir + "/fonts/Roboto-Regular.ttf";
    const std::string eatPath = assetsDir + "/sfx/apple.wav";
    const std::string losePath = assetsDir + "/sfx/lose.wav";
    const std::string winPath = assetsDir + "/sfx/win.wav";
    const std::string bonusPath = assetsDir + "/sfx/bonus.wav";
    const std::string breakPath = assetsDir + "/sfx/break.wav";
    const std::string portalPath = assetsDir + "/sfx/portal.wav";
    const std::string sfxuihitPath = assetsDir + "/sfx/menu_select.wav";
    const std::string sfxuimovePath = assetsDir + "/sfx/hit.wav";

    bool ok = true;

    ok &= ensureExists(fontPath, "font");
    ok &= ensureExists(eatPath, "sound");
    ok &= ensureExists(losePath, "sound");
    ok &= ensureExists(winPath, "sound");
    ok &= ensureExists(bonusPath, "sound");
    ok &= ensureExists(breakPath, "sound");
    ok &= ensureExists(portalPath, "sound");
    ok &= ensureExists(sfxuihitPath, "sound");
    ok &= ensureExists(sfxuimovePath, "sound");
    if (!ok) 
    {
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
    if (!win_->loadFromFile(winPath))
    {
        std::cerr << "[RES] SFML failed to load sound: " << winPath << "\n";
        ok = false;
    }
    if (!bonus_->loadFromFile(bonusPath))
    {
        std::cerr << "[RES] SFML failed to load sound: " << bonusPath << "\n";
        ok = false;
    }
    if (!break_->loadFromFile(breakPath))
    {
        std::cerr << "[RES] SFML failed to load sound: " << breakPath << "\n";
        ok = false;
    }
    if (!portal_->loadFromFile(portalPath))
    {
        std::cerr << "[RES] SFML failed to load sound: " << portalPath << "\n";
        ok = false;
    }
    if (!sfxUiHitBuf_->loadFromFile(sfxuihitPath))
    {
        std::cerr << "[RES] SFML failed to load sound: " << sfxuihitPath << "\n";
        ok = false;
    }
    if (!sfxUiMoveBuf_->loadFromFile(sfxuimovePath))
    {
        std::cerr << "[RES] SFML failed to load sound: " << sfxuimovePath << "\n";
        ok = false;
    }

    if (!ok) 
    {
        std::cerr << "[RES] Resource loading FAILED (SFX/FONT). "
        "Will still try to load textures…\n";
    }
    auto loadTex = [&](std::unique_ptr<sf::Texture>& t, const std::string& path) 
    {
        t.reset(new sf::Texture());
        t->setSmooth(true);
        if (!t->loadFromFile(path)) 
        {
            std::cerr << "[RES] SFML failed to load texture: " << path << "\n";
            ok = false;
        }
    };

    const std::string G = "assets/tex/";
    loadTex(txSnakeHead_, G + "sHead.png");
    loadTex(txSnakeBody_, G + "sBody.png");
    loadTex(txSnakeTail_, G + "sTail.png");
    loadTex(txApple1_, G + "apple.png");
    loadTex(txApple2_, G + "apple2.png");
    loadTex(txApple3_, G + "apple3.png");
    loadTex(txPowerBomb_, G + "bomb.png");
    loadTex(txPowerMush_, G + "mush.png");
    loadTex(txExplosion_, G + "expl.png");
    loadTex(txGround_, G + "ground.png");
    loadTex(txWall_, G + "wall.png");
    loadTex(txObstacle_, G + "obs.png");

    return ok;
}

void Resources::stopMusic() { music_.stop(); }

void Resources::playSfx(sf::Sound& s, float volume) 
{
    if (!soundOn_) return; // mute
    s.setVolume(volume);
    s.play();
}