#pragma once
class CameraShake 
{
public:
    void start(float duration, float amplitude) { t_ = duration; amp_ = amplitude; }
    void update(float dt) { if (t_ > 0) t_ -= dt; }
    float offsetX() const;
    float offsetY() const;
    bool active() const { return t_ > 0; }
private:
    float t_{ 0 }, amp_{ 0 };
};