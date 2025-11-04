#pragma once
#include <utility>
class CameraShake
{
public:
    void start(float duration, float amplitude) { t_ = duration; amp_ = amplitude; }
    void startAdd(float duration, float amplitude) { t_ = (t_ < duration ? duration : t_); amp_ += amplitude; }
    void update(float dt) { if (t_ > 0) t_ -= dt; }

    float offsetX() const;
    float offsetY() const;

    std::pair<float, float> sample() const { return { offsetX(), offsetY() }; }

    bool active() const { return t_ > 0; }

    void setFreq(float fx, float fy) { freqX_ = fx; freqY_ = fy; }
    void setPhase(float px, float py) { phaseX_ = px; phaseY_ = py; }
    float amp()  const { return amp_; }
    float time() const { return t_; }

private:
    float t_{ 0 }, amp_{ 0 };
    float freqX_{ 70.f }, freqY_{ 65.f };
    float phaseX_{ 0.f }, phaseY_{ 0.f };
};