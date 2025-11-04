#include "core/Resources.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

static bool ensureExists(const fs::path& path, const char* kind)
{
    if (!fs::exists(path))
    {
        std::cerr << "[RES] Missing " << kind << ": " << path.string() << "\n"
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
        if (music_.getStatus() == sf::Music::Playing)
            music_.pause();
        return;
    }

    // music on
    if (music_.getStatus() == sf::Music::Paused)
    {
        music_.play();
        return;
    }

    if (!currentPath_.empty())
    {
        const bool loop =
            (currentTrack_ == MusicTrack::Menu || currentTrack_ == MusicTrack::Session);
        openAndPlay_(currentPath_, loop, music_.getVolume(), currentTrack_);
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

sf::Music* Resources::music()
{
    return musicOn_ ? &music_ : nullptr;
}

bool Resources::load(const std::string& assetsDir)
{
    const fs::path root = fs::path(assetsDir);
    const fs::path fontPath = root / "fonts/Roboto-Regular.ttf";
    const fs::path eatPath = root / "sfx/apple.wav";
    const fs::path losePath = root / "sfx/lose.wav";
    const fs::path winPath = root / "sfx/win.wav";
    const fs::path bonusPath = root / "sfx/bonus.wav";
    const fs::path breakPath = root / "sfx/break.wav";
    const fs::path portalPath = root / "sfx/portal.wav";
    const fs::path sfxuihitPath = root / "sfx/menu_select.wav";
    const fs::path sfxuimovePath = root / "sfx/hit.wav";

    bool ok = true;
    struct Need { fs::path p; const char* kind; };
    const Need needed[] = 
    {
        { fontPath,     "font"  },
        { eatPath,      "sound" },
        { losePath,     "sound" },
        { winPath,      "sound" },
        { bonusPath,    "sound" },
        { breakPath,    "sound" },
        { portalPath,   "sound" },
        { sfxuihitPath, "sound" },
        { sfxuimovePath,"sound" },
    };

    for (const auto& n : needed)
        ok &= ensureExists(n.p, n.kind);

    if (!ok) 
    {
        std::cerr << "[RES] One or more asset files are missing. Fix paths or copy assets next to the executable.\n";
        return false;
    }
    const fs::path G = root / "tex";

    auto loadFont = [&](std::unique_ptr<sf::Font>& dst, const fs::path& p)
    {
        if (!dst) dst.reset(new sf::Font());
        if (!dst->loadFromFile(p.string()))
        {
            std::cerr << "[RES] SFML failed to load font: " << p.string() << "\n";
            ok = false;
        }
    };

    bool sfxOk = true;
    auto loadBufTracked = [&](std::unique_ptr<sf::SoundBuffer>& dst, const fs::path& p)
    {
        if (!dst) dst.reset(new sf::SoundBuffer());
        if (!dst->loadFromFile(p.string()))
        {
            std::cerr << "[RES] SFML failed to load sound: " << p.string() << "\n";
            ok = false;
            sfxOk = false;
        }
    };

    loadFont(font_, fontPath);
    loadBufTracked(eat_, eatPath);
    loadBufTracked(death_, losePath);
    loadBufTracked(win_, winPath);
    loadBufTracked(bonus_, bonusPath);
    loadBufTracked(break_, breakPath);
    loadBufTracked(portal_, portalPath);
    loadBufTracked(sfxUiHitBuf_, sfxuihitPath);
    loadBufTracked(sfxUiMoveBuf_, sfxuimovePath);

    if (!ok)
    {
        std::cerr << "[RES] Resource loading FAILED (SFX/FONT). "
            "Will still try to load textures…\n";
    }

    auto loadTex = [&](std::unique_ptr<sf::Texture>& t, const fs::path& p)
    {
        t.reset(new sf::Texture());
        t->setSmooth(true);
        if (!t->loadFromFile(p.string()))
        {
            std::cerr << "[RES] SFML failed to load texture: " << p.string() << "\n";
            ok = false;
        }
    };

    auto loadTexRaw = [&](sf::Texture& t, const fs::path& p)
    {
        t.setSmooth(true);
        if (!t.loadFromFile(p.string()))
        {
            std::cerr << "[RES] SFML failed to load texture: " << p.string() << "\n";
            ok = false;
        }
    };

    struct TexDef { std::unique_ptr<sf::Texture>* slot; const char* file; bool noSmooth = false; bool repeated = false; };
    const TexDef texList[] = 
    {
        { &txSnakeHead_,  "sHead.png"   },
        { &txSnakeBody_,  "sBody.png"   },
        { &txSnakeTail_,  "sTail.png"   },
        { &txBodyC1_,     "sBodyC1.png" },
        { &txBodyC2_,     "sBodyC2.png" },
        { &txBodyC3_,     "sBodyC3.png" },
        { &txBodyC4_,     "sBodyC4.png" },
        { &txApple1_,     "apple.png"   },
        { &txApple2_,     "apple2.png"  },
        { &txApple3_,     "apple3.png"  },
        { &txPowerBomb_,  "bomb.png"    },
        { &txPowerMush_,  "mush.png"    },
        { &txExplosion_,  "expl.png"    },
        { &txGround_,     "ground.png",  true,  true },
        { &txWall_,       "wall.png",    true,  false },
        { &txObstacle_,   "obs.png",     true,  false },
    };
        for (const auto& td : texList)
    {
        loadTex(*td.slot, G / td.file);
        if (td.noSmooth) (*td.slot)->setSmooth(false);
        if (td.repeated) (*td.slot)->setRepeated(true);
    }

    {
        const char* names[8] = 
        { "prtl1.png","prtl2.png","prtl3.png","prtl4.png",
        "prtl5.png","prtl6.png","prtl7.png","prtl8.png" };
        for (size_t i = 0; i < 8; ++i)
        loadTexRaw(txPortal_[i], G / names[i]);
    }

    soundOn_ = sfxOk;
    return ok;
}

void Resources::stopMusic() { music_.stop(); }

void Resources::playSfx(sf::Sound& s, float volume)
{
    if (!soundOn_) return;
    s.stop();
    s.setVolume(volume);
    s.setRelativeToListener(true);
    s.play();
}