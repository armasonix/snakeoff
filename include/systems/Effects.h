#pragma once
#include <algorithm>

class Effects
{
public:
    void reset() { speedMul_ = 1.f; invTime_ = 0.f; spdTime_ = 0.f; }

    // timers tick; dt seconds
    void update(float dt) 
    {
        if (invTime_ > 0.f) invTime_ = std::max(0.f, invTime_ - dt);
        if (spdTime_ > 0.f) 
        {
            spdTime_ = std::max(0.f, spdTime_ - dt);
            if (spdTime_ == 0.f) speedMul_ = 1.f;
        }
    }

    // applying effects
    void applyInvert(float durationSec) { invTime_ = std::max(invTime_, durationSec); }
    void applySpeed(float mul, float durationSec) 
    {
        speedMul_ = std::max(speedMul_, mul);
        spdTime_ = std::max(spdTime_, durationSec);
    }

    // listen the state
    bool  inverted()   const { return invTime_ > 0.f; }
    float invRemain()  const { return invTime_; }
    float speedMul()   const { return speedMul_; }
    float spdRemain()  const { return spdTime_; }

private:
    float invTime_{ 0.f };
    float speedMul_{ 1.f };
    float spdTime_{ 0.f };
};