#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio.hpp> 
#include <memory>
#include <string>

enum class MusicTrack { None, Menu, Session, GameOver };

class Resources 
{
public:
    bool load(const std::string& assetsDir);
    const sf::Font& font() const { return *font_; }
    const sf::SoundBuffer& sfxEat() const { return *eat_; }
    const sf::SoundBuffer& sfxDeath() const { return *death_; }
    const sf::SoundBuffer& sfxUiMove() const { return *sfxUiMoveBuf_; }
    const sf::SoundBuffer& sfxUiHit()  const { return *sfxUiHitBuf_; }

    void setSoundEnabled(bool on);
    void setMusicEnabled(bool on);
    bool soundEnabled() const { return soundOn_; }
    bool musicEnabled() const { return musicOn_; }

    void stopMusic();

    void ensureMenuLoop(float vol = 45.f);
    void ensureSessionLoop(float vol = 45.f);
    void switchToGameOver(float vol = 45.f);

    void pauseMusic();
    void resumeMusic();

    MusicTrack currentTrack() const { return currentTrack_; }
    sf::SoundSource::Status musicStatus() const { return music_.getStatus(); }

    void playSfx(sf::Sound& s, float volume = 100.f);

private:
    std::unique_ptr<sf::Font> font_;
    std::unique_ptr<sf::SoundBuffer> eat_, death_, sfxUiMoveBuf_, sfxUiHitBuf_;

    bool soundOn_ = true;
    bool musicOn_ = true;
    sf::Music music_;
    MusicTrack currentTrack_ = MusicTrack::None;
    std::string currentPath_;

    bool openAndPlay_(const std::string& path, bool loop, float vol, MusicTrack t);
};