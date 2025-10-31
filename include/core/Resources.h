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
    const sf::SoundBuffer& sfxWin() const { return *win_; }
    const sf::SoundBuffer& sfxBonus() const { return *bonus_; }
    const sf::SoundBuffer& sfxBreak() const { return *break_; }
    const sf::SoundBuffer& sfxPortal() const { return *portal_; }
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

    // texture sprites
    const sf::Texture & txSnakeHead() const { return *txSnakeHead_; }
    const sf::Texture & txSnakeBody() const { return *txSnakeBody_; }
    const sf::Texture & txSnakeTail() const { return *txSnakeTail_; }
    const sf::Texture & txApple1()    const { return *txApple1_; }
    const sf::Texture & txApple2()    const { return *txApple2_; }
    const sf::Texture & txApple3()    const { return *txApple3_; }
    const sf::Texture & txPowerBomb() const { return *txPowerBomb_; }
    const sf::Texture & txPowerMush() const { return *txPowerMush_; }
    const sf::Texture & txExplosion() const { return *txExplosion_; }
    const sf::Texture & txGround()    const { return *txGround_; }
    const sf::Texture & txWall()      const { return *txWall_; }
    const sf::Texture & txObstacle()  const { return *txObstacle_; }
    // corners
    const sf::Texture & txBodyC1() const { return *txBodyC1_; }
    const sf::Texture & txBodyC2() const { return *txBodyC2_; }
    const sf::Texture & txBodyC3() const { return *txBodyC3_; }
    const sf::Texture & txBodyC4() const { return *txBodyC4_; }

private:
    std::unique_ptr<sf::Font> font_;
    std::unique_ptr<sf::SoundBuffer> eat_, death_, sfxUiMoveBuf_, sfxUiHitBuf_, win_, bonus_, break_, portal_;

    bool soundOn_ = true;
    bool musicOn_ = true;
    sf::Music music_;
    MusicTrack currentTrack_ = MusicTrack::None;
    std::string currentPath_;

    bool openAndPlay_(const std::string& path, bool loop, float vol, MusicTrack t);

    std::unique_ptr<sf::Texture> txSnakeHead_;
    std::unique_ptr<sf::Texture> txSnakeBody_;
    std::unique_ptr<sf::Texture> txSnakeTail_;
    std::unique_ptr<sf::Texture> txApple1_;
    std::unique_ptr<sf::Texture> txApple2_;
    std::unique_ptr<sf::Texture> txApple3_;
    std::unique_ptr<sf::Texture> txPowerBomb_;
    std::unique_ptr<sf::Texture> txPowerMush_;
    std::unique_ptr<sf::Texture> txExplosion_;
    std::unique_ptr<sf::Texture> txGround_;
    std::unique_ptr<sf::Texture> txWall_;
    std::unique_ptr<sf::Texture> txObstacle_;
    // corners
    std::unique_ptr<sf::Texture> txBodyC1_;
    std::unique_ptr<sf::Texture> txBodyC2_;
    std::unique_ptr<sf::Texture> txBodyC3_;
    std::unique_ptr<sf::Texture> txBodyC4_;
};