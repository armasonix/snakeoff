#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio.hpp> 
#include <memory>
#include <string>

class Resources 
{
public:
    bool load(const std::string& assetsDir);
    const sf::Font& font() const { return *font_; }
    const sf::SoundBuffer& sfxEat() const { return *eat_; }
    const sf::SoundBuffer& sfxDeath() const { return *death_; }

    void setSoundEnabled(bool on);
    void setMusicEnabled(bool on);
    bool soundEnabled() const { return soundOn_; }
    bool musicEnabled() const { return musicOn_; }

    void playMenuMusic();
    void playSessionMusic();
    void stopMusic();

    void playSfx(sf::Sound& s, float volume = 100.f);

private:
    std::unique_ptr<sf::Font> font_;
    std::unique_ptr<sf::SoundBuffer> eat_, death_;

    bool soundOn_ = true;
    bool musicOn_ = true;
    sf::Music music_;
};